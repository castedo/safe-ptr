#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include "safe_ptr.hpp"

#include <functional>

using namespace spl;

struct number
{
  int i;

  number() : i(1) {}
  number(int j) : i(j) {}
  number(int j, int k) : i(j+k) {}
  number(int j, int k, int l, int m, int n) : i(j+k+l+m+n) {}
  number(int j, int k, int l, int m, int n, int o) : i(j+k+l+m+n+o) {}
};

BOOST_AUTO_TEST_CASE( test_shared_ptr_to_safe_ptr )
{
  std::shared_ptr<number> p(new number());
  safe_ptr<number> q(p);
  BOOST_CHECK_EQUAL(q->i, 1);
}

BOOST_AUTO_TEST_CASE( test_make_shared_to_safe_ptr )
{
  safe_ptr<number> p(std::make_shared<number>());
  BOOST_CHECK_EQUAL(p->i, 1);
}

BOOST_AUTO_TEST_CASE( test_safe_ptr_default )
{
  safe_ptr<number> p;
  BOOST_CHECK_EQUAL(p->i, 1);
}

BOOST_AUTO_TEST_CASE( test_make_safe )
{
  safe_ptr<number> p(make_safe<number>(2));
  BOOST_CHECK_EQUAL(p->i, 2);

  p = make_safe<number>(1,2);
  BOOST_CHECK_EQUAL(p->i, 3);

  p = make_safe<number>(1,2,3,4,5);
  BOOST_CHECK_EQUAL(p->i, 15);

#ifdef SPL_HAS_VARIADIC_TEMPLATES 
  p = make_safe<number>(1,2,3,4,5,6);
  BOOST_CHECK_EQUAL(p->i, 21);
#endif
}


class event_notifier
{
    std::function<void (int)> event_handler_;
  public:
    void notify_me(std::function<void (int)> handler)
    {
      event_handler_ = handler;
    }
    void fire_event(int i)
    {
      if (event_handler_) { event_handler_(i); }
    }
};

class event_handler : public enable_safe_from_this<event_handler>
{
    int i_;

    void handle_event(int i)
    {
      i_ = i;
    }

  public:
    event_handler() {}
    event_handler(int i) : i_(i) {}

    int i() { return i_; }

    void do_something(event_notifier & notifier)
    {
      using namespace std::placeholders;
      auto fun = std::bind(&event_handler::handle_event, safe_from_this(), _1);
      notifier.notify_me(fun);
    }
};

BOOST_AUTO_TEST_CASE( test_safe_this )
{
  std::weak_ptr<event_handler> weakie;
  {
    event_notifier notifie;
    {
      safe_ptr<event_handler> handie;
      handie = make_safe<event_handler>(101);
      BOOST_CHECK_EQUAL( handie->i(), 101 );
      handie->do_something(notifie);
      notifie.fire_event(42);
      BOOST_CHECK_EQUAL( handie->i(), 42 );
      weakie = handie;
    }
    BOOST_CHECK(!weakie.expired()); // weakie still finds handie is alive
    notifie.fire_event(13);  // will not crash
  }
  // but now handie has been cleaned up and not leaked
  BOOST_CHECK(weakie.expired());
}

