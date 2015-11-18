#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>

#if defined(__VARIADIC_TEMPLATES) || (defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)))
#define SPL_HAS_VARIADIC_TEMPLATES
#endif

namespace spl
{

template<typename T>
class safe_ptr
{
    template <typename> friend class safe_ptr;
public:
    typedef T  element_type;

    safe_ptr(); // will construct new T object using make_safe<T>()

    safe_ptr(const safe_ptr& other)
        : p_(other.p_)
    {
    }

    template<typename U>
    safe_ptr(const safe_ptr<U>& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type = 0)
        : p_(other.p_)
    {
    }

    template<typename U>
    explicit safe_ptr(const std::shared_ptr<U>& p, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type = 0)
        : p_(p)
    {
        if (!p)
            throw std::invalid_argument("p");
    }

    template<typename U>
    explicit safe_ptr(std::shared_ptr<U>&& p, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type = 0)
        : p_(std::move(p))
    {
        if (!p_)
            throw std::invalid_argument("p");
    }

    template<typename U>
    explicit safe_ptr(std::unique_ptr<U>&& p, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type = 0)
        : p_(std::move(p))
    {
        if (!p_)
            throw std::invalid_argument("p");
    }

    template<typename U>
    explicit safe_ptr(U* p, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type = 0)
        : p_(p)
    {
        if (!p)
            throw std::invalid_argument("p");
    }

    template<typename U, typename D>
    explicit safe_ptr(U* p, D d, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type = 0)
        : p_(p, d)
    {
        if (!p)
            throw std::invalid_argument("p");
    }

    template<typename U>
    safe_ptr(safe_ptr<U> const& own, T * p)
        : p_(static_cast<std::shared_ptr<U>>(own), p)
    {
        if (!p)
            throw std::invalid_argument("p");
    }

    template<typename U>
    typename std::enable_if<std::is_convertible<U*, T*>::value, safe_ptr&>::type
    operator=(const safe_ptr<U>& other)
    {
        safe_ptr(other).swap(*this);
        return *this;
    }

    template<typename U>
    typename std::enable_if<std::is_convertible<U*, T*>::value, safe_ptr&>::type
    operator=(safe_ptr<U>&& other)
    {
        safe_ptr<T>(std::move(other)).swap(*this);
        return *this;
    }

    T& operator*() const
    {
        return *p_.get();
    }

    T* operator->() const
    {
        return p_.get();
    }

    T* get() const
    {
        return p_.get();
    }

    bool unique() const
    {
        return p_.unique();
    }

    long use_count() const
    {
        return p_.use_count();
    }

    void swap(safe_ptr& other)
    {
        p_.swap(other.p_);
    }

    operator std::shared_ptr<T>() const
    {
        return p_;
    }

    operator std::weak_ptr<T>() const
    {
        return std::weak_ptr<T>(p_);
    }

    template<typename D, typename = typename
        std::enable_if<std::is_convertible<T*, D*>::value>::type>
    operator std::shared_ptr<D>() const
    {
        return p_;
    }

    template<typename D, typename = typename
        std::enable_if<std::is_convertible<T*, D*>::value>::type>
    operator std::weak_ptr<D>() const
    {
        return std::weak_ptr<D>(p_);
    }

    template<class U>
    bool owner_before(const safe_ptr& ptr)
    {
        return p_.owner_before(ptr.p_);
    }

    template<class U>
    bool owner_before(const std::shared_ptr<U>& ptr)
    {
        return p_.owner_before(ptr);
    }

    template<class D, class U>
    D* get_deleter(safe_ptr<U> const& ptr)
    {
        return p_.get_deleter();
    }

private:
    std::shared_ptr<T> p_;
};

template<class T, class U>
bool operator==(const safe_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() == b.get();
}

template<class T, class U>
bool operator==(const std::shared_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() == b.get();
}

template<class T, class U>
bool operator==(const safe_ptr<T>& a, const std::shared_ptr<U>& b)
{
    return a.get() == b.get();
}

template<class T, class U>
bool operator!=(const safe_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() != b.get();
}

template<class T, class U>
bool operator!=(const std::shared_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() != b.get();
}

template<class T, class U>
bool operator!=(const safe_ptr<T>& a, const std::shared_ptr<U>& b)
{
    return a.get() != b.get();
}

template<class T, class U>
bool operator<(const safe_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() < b.get();
}

template<class T, class U>
bool operator<(const std::shared_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() < b.get();
}

template<class T, class U>
bool operator<(const safe_ptr<T>& a, const std::shared_ptr<U>& b)
{
    return a.get() < b.get();
}

template<class T, class U>
bool operator>(const safe_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() > b.get();
}

template<class T, class U>
bool operator>(const std::shared_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() > b.get();
}

template<class T, class U>
bool operator>(const safe_ptr<T>& a, const std::shared_ptr<U>& b)
{
    return a.get() > b.get();
}

template<class T, class U>
bool operator>=(const safe_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() >= b.get();
}

template<class T, class U>
bool operator>=(const std::shared_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() >= b.get();
}

template<class T, class U>
bool operator>=(const safe_ptr<T>& a, const std::shared_ptr<U>& b)
{
    return a.get() >= b.get();
}

template<class T, class U>
bool operator<=(const safe_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() <= b.get();
}

template<class T, class U>
bool operator<=(const std::shared_ptr<T>& a, const safe_ptr<U>& b)
{
    return a.get() <= b.get();
}

template<class T, class U>
bool operator<=(const safe_ptr<T>& a, const std::shared_ptr<U>& b)
{
    return a.get() <= b.get();
}

template<class E, class T, class U>
std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& out, const safe_ptr<U>& p)
{
    return out << p.get();
}

template<class T>
void swap(safe_ptr<T>& a, safe_ptr<T>& b)
{
    a.swap(b);
}

template<class T>
T* get_pointer(safe_ptr<T> const& p)
{
    return p.get();
}

template <class T, class U>
safe_ptr<T> static_pointer_cast(const safe_ptr<U>& p)
{
    return safe_ptr<T>(std::static_pointer_cast<T>(std::shared_ptr<U>(p)));
}

template <class T, class U>
safe_ptr<T> const_pointer_cast(const safe_ptr<U>& p)
{
    return safe_ptr<T>(std::const_pointer_cast<T>(std::shared_ptr<U>(p)));
}

template <class T, class U>
safe_ptr<T> dynamic_pointer_cast(const safe_ptr<U>& p)
{
    auto temp = std::dynamic_pointer_cast<T>(std::shared_ptr<U>(p));
    if(!temp)
        throw std::bad_cast();
    return safe_ptr<T>(std::move(temp));
}

//
// enable_safe_this
//
// A safe_ptr version of enable_shared_from_this.
// So that an object may get safe_ptr objects to itself.
//

template<class T>
class enable_safe_from_this : public std::enable_shared_from_this<T>
{
public:
    safe_ptr<T> safe_from_this()
    {
        return safe_ptr<T>(this->shared_from_this());
    }

    safe_ptr<T const> safe_from_this() const
    {
        return safe_ptr<T const>(this->shared_from_this());
    }
protected:
    enable_safe_from_this()
    {
    }

    enable_safe_from_this(const enable_safe_from_this&)
    {
    }

    enable_safe_from_this& operator=(const enable_safe_from_this&)
    {
        return *this;
    }

    ~enable_safe_from_this ()
    {
    }
};

namespace detail
{
    template<typename T>
    T && forward_type();

    struct specialization_tag {};

    template<class MakeSafeReturnType>
    struct specialize_if_exist
    {
        typedef specialization_tag type;

        static_assert( std::is_same<MakeSafeReturnType,
                                    safe_ptr<typename MakeSafeReturnType::element_type>
                       >::value, "make_safe should return a safe_ptr" );
    };

    #define DEFAULT_MAKE_SAFE_IMPL(Args___args, forward___)                       \
    {                                                                             \
        static safe_ptr<T> make_safe(Args___args)                                 \
        {                                                                         \
            return safe_ptr<T>(std::make_shared<T>(forward___));                  \
        }                                                                         \
    };

    // specialization tag for when appropriate T::make_safe exists
    #define SPECIALIZATION_TAG(forward_type___)                                   \
    typename specialize_if_exist<decltype(T::make_safe(forward_type___)) >::type

    #define SPECIAL_MAKE_SAFE_IMPL(Args___args, forward___)                       \
    {                                                                             \
        static safe_ptr<T> make_safe(Args___args)                                 \
        {                                                                         \
            return T::make_safe(forward___);                                      \
        }                                                                         \
    };

    #ifdef SPL_HAS_VARIADIC_TEMPLATES

    // default implementation when appropriate T::make_safe does not exist
    template<typename SpecializationTag, typename T, typename... Args>
    struct make_safe_impl
    DEFAULT_MAKE_SAFE_IMPL(Args&&... args, std::forward<Args>(args)...);

    // specialization if appropriate T::make_safe exists
    template<typename T, typename... Args>
    struct make_safe_impl<SPECIALIZATION_TAG(forward_type<Args>()...), T, Args...>
    SPECIAL_MAKE_SAFE_IMPL(Args&&... args, std::forward<Args>(args)...);

    #else

    namespace t0 {
        #define ARGS___ARGS
        #define FORWARD___
        #define FORWARDTYPE___

        template<typename SpecializationTag, typename T>
        struct make_safe_impl
        DEFAULT_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        template<typename T>
        struct make_safe_impl<SPECIALIZATION_TAG(FORWARDTYPE___), T>
        SPECIAL_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        #undef ARGS___ARGS
        #undef FORWARD___
        #undef FORWARDTYPE___
    }

    namespace t1 {
        #define ARGS___            A0
        #define TYPENAME___ARGS    typename A0
        #define ARGS___ARGS        A0&& a0
        #define FORWARD___         std::forward<A0>(a0)
        #define FORWARDTYPE___     forward_type<A0>()

        template<typename SpecializationTag, typename T, TYPENAME___ARGS>
        struct make_safe_impl
        DEFAULT_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        template<typename T, TYPENAME___ARGS>
        struct make_safe_impl<SPECIALIZATION_TAG(FORWARDTYPE___), T, ARGS___>
        SPECIAL_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        #undef ARGS___
        #undef TYPENAME___ARGS
        #undef ARGS___ARGS
        #undef FORWARD___
        #undef FORWARDTYPE___
    }

    namespace t2 {
        #define ARGS___            A0, A1
        #define TYPENAME___ARGS    typename A0, typename A1
        #define ARGS___ARGS        A0&& a0, A1&& a1
        #define FORWARD___         std::forward<A0>(a0), std::forward<A1>(a1)
        #define FORWARDTYPE___     forward_type<A0>(), forward_type<A1>()

        template<typename SpecializationTag, typename T, TYPENAME___ARGS>
        struct make_safe_impl
        DEFAULT_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        template<typename T, TYPENAME___ARGS>
        struct make_safe_impl<SPECIALIZATION_TAG(FORWARDTYPE___), T, ARGS___>
        SPECIAL_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        #undef ARGS___
        #undef TYPENAME___ARGS
        #undef ARGS___ARGS
        #undef FORWARD___
        #undef FORWARDTYPE___
    }

    namespace t3 {
        #define ARGS___            A0, A1, A2
        #define TYPENAME___ARGS    typename A0, typename A1, typename A2
        #define ARGS___ARGS        A0&& a0, A1&& a1, A2&& a2
        #define FORWARD___         std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2)
        #define FORWARDTYPE___     forward_type<A0>(), forward_type<A1>(), forward_type<A2>()

        template<typename SpecializationTag, typename T, TYPENAME___ARGS>
        struct make_safe_impl
        DEFAULT_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        template<typename T, TYPENAME___ARGS>
        struct make_safe_impl<SPECIALIZATION_TAG(FORWARDTYPE___), T, ARGS___>
        SPECIAL_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        #undef ARGS___
        #undef TYPENAME___ARGS
        #undef ARGS___ARGS
        #undef FORWARD___
        #undef FORWARDTYPE___
    }

    namespace t4 {
        #define ARGS___            A0, A1, A2, A3
        #define TYPENAME___ARGS    typename A0, typename A1, typename A2, typename A3
        #define ARGS___ARGS        A0&& a0, A1&& a1, A2&& a2, A3&& a3
        #define FORWARD___         std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3)
        #define FORWARDTYPE___     forward_type<A0>(), forward_type<A1>(), forward_type<A2>(), forward_type<A3>()

        template<typename SpecializationTag, typename T, TYPENAME___ARGS>
        struct make_safe_impl
        DEFAULT_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        template<typename T, TYPENAME___ARGS>
        struct make_safe_impl<SPECIALIZATION_TAG(FORWARDTYPE___), T, ARGS___>
        SPECIAL_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        #undef ARGS___
        #undef TYPENAME___ARGS
        #undef ARGS___ARGS
        #undef FORWARD___
        #undef FORWARDTYPE___
    }

    namespace t5 {
        #define ARGS___            A0, A1, A2, A3, A4
        #define TYPENAME___ARGS    typename A0, typename A1, typename A2, typename A3, typename A4
        #define ARGS___ARGS        A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4
        #define FORWARD___         std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4)
        #define FORWARDTYPE___     forward_type<A0>(), forward_type<A1>(), forward_type<A2>(), forward_type<A3>(), forward_type<A4>()

        template<typename SpecializationTag, typename T, TYPENAME___ARGS>
        struct make_safe_impl
        DEFAULT_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        template<typename T, TYPENAME___ARGS>
        struct make_safe_impl<SPECIALIZATION_TAG(FORWARDTYPE___), T, ARGS___>
        SPECIAL_MAKE_SAFE_IMPL(ARGS___ARGS, FORWARD___);

        #undef ARGS___
        #undef TYPENAME___ARGS
        #undef ARGS___ARGS
        #undef FORWARD___
        #undef FORWARDTYPE___
    }

    #endif

    #undef DEFAULT_MAKE_SAFE_IMPL
    #undef SPECIAL_MAKE_SAFE_IMPL

} // namespace detail

//
// make_safe
//
// safe_ptr equivalents to make_shared
//

#ifdef SPL_HAS_VARIADIC_TEMPLATES

template<typename T, typename... Args>
safe_ptr<T> make_safe(Args&&... args)
{
    return detail::make_safe_impl<detail::specialization_tag, T, Args...>::make_safe(std::forward<Args>(args)...);
}

#else

template<typename T>
safe_ptr<T> make_safe()
{
    return detail::t0::make_safe_impl<detail::specialization_tag, T>::make_safe();
}

template<typename T, typename P0>
safe_ptr<T> make_safe(P0&& p0)
{
    return detail::t1::make_safe_impl<detail::specialization_tag, T, P0>::make_safe(std::forward<P0>(p0));
}

template<typename T, typename P0, typename P1>
safe_ptr<T> make_safe(P0&& p0, P1&& p1)
{
    return detail::t2::make_safe_impl<detail::specialization_tag, T, P0, P1>::make_safe(std::forward<P0>(p0), std::forward<P1>(p1));
}

template<typename T, typename P0, typename P1, typename P2>
safe_ptr<T> make_safe(P0&& p0, P1&& p1, P2&& p2)
{
    return detail::t3::make_safe_impl<detail::specialization_tag, T, P0, P1, P2>::make_safe(std::forward<P0>(p0), std::forward<P1>(p1), std::forward<P2>(p2));
}

template<typename T, typename P0, typename P1, typename P2, typename P3>
safe_ptr<T> make_safe(P0&& p0, P1&& p1, P2&& p2, P3&& p3)
{
    return detail::t4::make_safe_impl<detail::specialization_tag, T, P0, P1, P2, P3>::make_safe(std::forward<P0>(p0), std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3));
}

template<typename T, typename P0, typename P1, typename P2, typename P3, typename P4>
safe_ptr<T> make_safe(P0&& p0, P1&& p1, P2&& p2, P3&& p3, P4&& p4)
{
    return detail::t5::make_safe_impl<detail::specialization_tag, T, P0, P1, P2, P3, P4>::make_safe(std::forward<P0>(p0), std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4));
}

#endif

template<typename T>
safe_ptr<T>::safe_ptr()
    : p_(make_safe<T>())
{
}

} // namespace

