#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "safe_ptr.hpp"

using namespace spl;

class noncopyable // copy of boost::noncopyable
{
   protected:
      noncopyable() {}
      ~noncopyable() {}
   private:  // emphasize the following members are private
      noncopyable( const noncopyable& );
      const noncopyable& operator=( const noncopyable& );
};

#ifdef SPL_HAS_VARIADIC_TEMPLATES 

class make_safe_only_0 : noncopyable
{
    make_safe_only_0() {}
    make_safe_only_0(int i) {}
public:
    template<class... Args>
    static safe_ptr<make_safe_only_0> make_safe(Args&&... args) {
        return safe_ptr<make_safe_only_0>(new make_safe_only_0(std::forward<Args>(args)...));
    }
};

template<typename T>
struct make_safe_by_new
{
    template<class... Args>
    static
    safe_ptr<T> make_safe(Args&&... args) {
        // std:::make_shared might not be able to construct since constructor is probably private.
        return safe_ptr<T>(new T(std::forward<Args>(args)...));
    }
};

class make_safe_only_1 : noncopyable, public make_safe_by_new<make_safe_only_1>
{
  make_safe_only_1() {}
  make_safe_only_1(int i) {}
  friend class make_safe_by_new<make_safe_only_1>;
};

class make_safe_key;

template<typename T>
struct make_safe_by_key
{
    template<class... Args>
    static
    safe_ptr<T> make_safe(Args&&... args) {
        return safe_ptr<T>(std::make_shared<T>(make_safe_key(), std::forward<Args>(args)...));
    }
};

class make_safe_key
{
    template<class T> friend struct make_safe_by_key;
    make_safe_key() {} // only make_safe_with_key can construct
};

class make_safe_only_2 : noncopyable, public make_safe_by_key<make_safe_only_2>
{
  make_safe_only_2() = delete;
public:
  make_safe_only_2(make_safe_key) {}
  make_safe_only_2(make_safe_key, int i) {}
};

BOOST_AUTO_TEST_CASE( test_make_safe_only )
{
    auto o0 = make_safe<make_safe_only_0>(101);
    auto o1 = make_safe<make_safe_only_1>(101);
    auto o2 = make_safe<make_safe_only_2>(101);
}

#endif

class make_safe_only_3 : noncopyable
{
  make_safe_only_3() = delete;
  make_safe_only_3(int i, int j) {}
public:
  static safe_ptr<make_safe_only_3> make_safe(int i, int j)
  {
     return safe_ptr<make_safe_only_3>(new make_safe_only_3(i, j));
  }
};

BOOST_AUTO_TEST_CASE( test_make_safe_only_non_variadic )
{
    int i = 101;
    auto o3 = make_safe<make_safe_only_3>(i, i);
    o3 = make_safe<make_safe_only_3>(4, 4);
    const int k = 101;
    o3 = make_safe<make_safe_only_3>(k, k);
}


