// Copyright (c) 2018 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef YOREL_YOMM2_INCLUDED
#define YOREL_YOMM2_INCLUDED

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>

#include <boost/type_traits/is_virtual_base_of.hpp>

#ifndef YOMM2_DEBUG
#ifdef NDEBUG
#define YOMM2_DEBUG 0
#else
#define YOMM2_DEBUG 1
#endif
#endif

#if YOMM2_DEBUG

#define _YOMM2_DEBUG(X) X
#define _YOMM2_COMMA_DEBUG(X) , X

#include <iostream>
#include <iterator>

#else
#define _YOMM2_DEBUG(ST)
#define _YOMM2_COMMA_DEBUG(X)
#endif

#define _YOMM2_NS BOOST_PP_CAT(_YOMM2_NS_, __COUNTER__)

#define _YOMM2_PLIST(N, I, A)                                                 \
    BOOST_PP_COMMA_IF(I)                                                      \
    ::yorel::yomm2::detail::virtual_arg_t<BOOST_PP_TUPLE_ELEM(I, A)>                  \
    BOOST_PP_CAT(a, I)

#define _YOMM2_ALIST(N, I, ARGS) \
    BOOST_PP_COMMA_IF(I) BOOST_PP_CAT(a, I)

#define _YOMM2_DECLARE_KEY(ID)                                                \
    BOOST_PP_CAT(_yomm2_method_, ID)

#define YOMM2_DECLARE(R, ID, ARGS) \
    YOMM2_DECLARE_(void, R, ID, ARGS)

#define _YOMM2_EXPAND(...) __VA_ARGS__

#define YOMM2_DECLARE_(REGISTRY, R, ID, ARGS)                                 \
    struct _YOMM2_DECLARE_KEY(ID);                                            \
    namespace {                                                               \
    namespace _YOMM2_NS {                                                     \
    ::yorel::yomm2::detail::method<REGISTRY,                                  \
                                   _YOMM2_DECLARE_KEY(ID), R ARGS>::init_method \
    init _YOMM2_DEBUG( = #ID  #ARGS ); } }                                    \
    ::yorel::yomm2::detail::method<REGISTRY,                                  \
                                   _YOMM2_DECLARE_KEY(ID), R ARGS> ID(        \
                                       ::yorel::yomm2::detail::discriminator, \
                                       BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(ARGS), \
                                                       _YOMM2_PLIST, ARGS));  \
    inline R ID(BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(ARGS),                    \
                                _YOMM2_PLIST, ARGS)) {                        \
        return reinterpret_cast<R (*)(                                        \
            BOOST_PP_REPEAT(                                                  \
                BOOST_PP_TUPLE_SIZE(ARGS),                                    \
                _YOMM2_PLIST, ARGS))>(                                        \
                    ::yorel::yomm2::detail::method<REGISTRY, _YOMM2_DECLARE_KEY(ID), R ARGS> \
                    ::resolve(BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(ARGS),      \
                                              _YOMM2_ALIST, ARGS)))           \
            (BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(ARGS),                       \
                             _YOMM2_ALIST, ARGS));                            \
    }

#define YOMM2_DEFINE(RETURN_T, ID, ARGS)                                      \
    namespace {                                                               \
    namespace _YOMM2_NS {                                                     \
    template<typename T> struct select_method;                                \
    template<typename... A> struct select_method<void(A...)> {                \
        using type = decltype(ID(::yorel::yomm2::detail::discriminator(),     \
                                 std::declval<A>()...));                      \
    };                                                                        \
    using _YOMM2_RETURN_T = RETURN_T;                                         \
    using _YOMM2_METHOD = select_method<void ARGS>::type;             \
    using _YOMM2_SIGNATURE = void ARGS;                                       \
    const char* _YOMM2_NAME = #ARGS;                                          \
    RETURN_T (*next)ARGS;                                                     \
    struct _YOMM2_SPEC {                                                      \
        static RETURN_T body ARGS

#define YOMM2_END                                                             \
    };                                                                        \
    ::yorel::yomm2::detail::register_spec<                                            \
       _YOMM2_RETURN_T, _YOMM2_METHOD, _YOMM2_SPEC, _YOMM2_SIGNATURE> \
    init((void**)&next _YOMM2_COMMA_DEBUG(_YOMM2_NAME));              \
    } }

#define _YOMM2_CLASS_NAME(CLASS, ...) \
    #CLASS

#define YOMM2_CLASS_(REG, ...)                                                \
    namespace {                                                               \
    namespace _YOMM2_NS {                                                     \
    ::yorel::yomm2::detail::                                                          \
    init_class_info<REG _YOMM2_CLASS_LIST(BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)) \
        > init _YOMM2_DEBUG( { _YOMM2_CLASS_NAME(__VA_ARGS__ ) } ); } }

#define YOMM2_CLASS(...)                                               \
    YOMM2_CLASS_(void, __VA_ARGS__)

#define _YOMM2_CLIST(N, I, A) \
    , BOOST_PP_TUPLE_ELEM(I, A)

#define _YOMM2_CLASS_LIST(TUPLE)                                              \
    BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(TUPLE),                               \
                    _YOMM2_CLIST, TUPLE)                                      \

namespace yorel {
namespace yomm2 {

template<typename T>
struct virtual_;

void update_methods();

struct method_call_error {
    enum type { not_implemented = 0, ambiguous = 1 } code;
    _YOMM2_DEBUG(const char* method_name);
};

using method_call_error_handler = void (*)(const method_call_error& error);

method_call_error_handler set_method_call_error_handler(method_call_error_handler handler);

namespace detail {

extern method_call_error_handler call_error_handler;

struct method_info;
struct class_info;

union word {
    void* pf;
    const word* pw;
    int i;
};

struct registry {
    std::vector<const class_info*> classes;
    std::vector<method_info*> methods;
    template<typename T> static registry& get();
};

template<typename T> registry& registry::get() {
    static registry r;
    return r;
}

struct dispatch_data {
    // global vector:
    std::vector<word> gv;
    std::uintptr_t hash_mult;
    std::size_t hash_shift;
    template<typename T> static dispatch_data instance;
};

template<typename T>
dispatch_data dispatch_data::instance;

struct dynamic_cast_ {};
struct static_cast_ {};

template<typename T>
struct virtual_traits {
    using base_type = typename std::remove_cv_t<std::remove_reference_t<T>>;
    using argument_type = T;
    template<typename>
    static T cast(T val, static_cast_) {
        return val;
    }
    template<typename>
    static T cast(T val, dynamic_cast_) {
        return val;
    }
};

template<typename T>
struct virtual_traits< virtual_<T&> > {
    using base_type = std::remove_cv_t<T>;
    using argument_type = T&;

    static_assert(std::is_class<base_type>::value);
    static_assert(std::is_polymorphic<base_type>::value);

    static auto key(argument_type arg) {
        return &typeid(arg);
    }

    template<class DERIVED>
    static DERIVED cast(T& obj, static_cast_) {
        return static_cast<DERIVED>(obj);
    }

    template<class DERIVED>
    static DERIVED cast(T& obj, dynamic_cast_) {
        return dynamic_cast<DERIVED>(obj);
    }
};

template<typename T>
struct virtual_traits< virtual_<T*> > {
    using base_type = std::remove_cv_t<T>;
    using argument_type = T*;

    static_assert(std::is_class<base_type>::value);
    static_assert(std::is_polymorphic<base_type>::value);

    static auto key(argument_type arg) {
        return &typeid(*arg);
    }

    template<class DERIVED>
    static DERIVED cast(T* obj, static_cast_) {
        static_assert(std::is_pointer<DERIVED>::value);
        return static_cast<DERIVED>(obj);
    }

    template<class DERIVED>
    static DERIVED cast(T* obj, dynamic_cast_) {
        static_assert(std::is_pointer<DERIVED>::value);
        return dynamic_cast<DERIVED>(obj);
    }
};

template<typename T>
struct shared_ptr_traits {
    static const bool is_shared_ptr = false;
};

template<typename T>
struct shared_ptr_traits< std::shared_ptr<T> > {
    static const bool is_shared_ptr = true;
    using base_type = T;
};

template<typename T>
struct virtual_traits< virtual_< std::shared_ptr<T> > > {
    using base_type = std::remove_cv_t<T>;
    using argument_type = std::shared_ptr<T>;

    static_assert(std::is_class<base_type>::value);
    static_assert(std::is_polymorphic<base_type>::value);

    static auto key(argument_type arg) {
        return &typeid(*arg);
    }

    template<class DERIVED>
    static DERIVED cast(argument_type obj, static_cast_) {
        static_assert(shared_ptr_traits<DERIVED>::is_shared_ptr);
        static_assert(
            std::is_class<typename shared_ptr_traits<DERIVED>::base_type>::value);
        return std::static_pointer_cast<typename shared_ptr_traits<DERIVED>::base_type>(obj);
    }

    template<class DERIVED>
    static DERIVED cast(argument_type obj, dynamic_cast_) {
        static_assert(shared_ptr_traits<DERIVED>::is_shared_ptr);
        static_assert(
            std::is_class<typename shared_ptr_traits<DERIVED>::base_type>::value);
        return std::dynamic_pointer_cast<typename shared_ptr_traits<DERIVED>::base_type>(obj);
    }
};

template<typename T>
using virtual_base_t = typename virtual_traits<T>::base_type;

template<typename T>
using virtual_arg_t = typename virtual_traits<T>::argument_type;

struct discriminator {};

inline std::size_t hash(const dispatch_data& t, const void* p) {
    return static_cast<std::size_t>(
        (t.hash_mult * reinterpret_cast<std::uintptr_t>(const_cast<void*>(p)))
        >> t.hash_shift);
}

inline const word* mptr(const dispatch_data& t, const std::type_info* ti) {
    return t.gv[hash(t, ti)].pw;
}

_YOMM2_DEBUG(std::ostream& log());
_YOMM2_DEBUG(std::ostream* log_on(std::ostream* os));
_YOMM2_DEBUG(std::ostream* log_off());

struct class_info {
    std::vector<const class_info*> direct_bases;
    _YOMM2_DEBUG(const char* name);
    std::unordered_set<const void*> ti_ptrs;

    template<typename REG, class CLASS> static class_info& get();
};

template<typename REG, class CLASS>
class_info& class_info::get() {
    static class_info info;
    return info;
}

template<typename REG, class CLASS, class... BASE>
struct init_class_info {
    init_class_info(_YOMM2_DEBUG(const char* name)) {
        auto& info = class_info::get<REG, CLASS>();
        static int called;
        if (!called++) {
            _YOMM2_DEBUG(info.name = name);
            registry::get<REG>().classes.push_back(&info);
            info.direct_bases = { &class_info::get<REG, BASE>()... };
        }
        auto inserted = info.ti_ptrs.insert(&typeid(CLASS));
        _YOMM2_DEBUG(
            if (inserted.second)
                ::yorel::yomm2::detail::log()
                      << "Register " << name
                      << " with &typeid " << &typeid(CLASS)
                      << " (" << typeid(CLASS).name() << ")"
                      << "\n");
    }
};

struct spec_info {
    _YOMM2_DEBUG(const char* name);
    std::vector<const class_info*> vp;
    void* pf;
    void** next;
};

struct method_info {
    _YOMM2_DEBUG(const char* name);
    std::vector<const class_info*> vp;
    std::vector<const spec_info*> specs;
    void* ambiguous;
    void* not_implemented;
    const word** slots_strides_p{nullptr};
};

template<typename BASE, typename DERIVED>
struct select_cast {
    using type = std::conditional_t<
        boost::is_virtual_base_of<BASE, DERIVED>::value, dynamic_cast_, static_cast_
    >;
};

template<typename BASE, typename DERIVED>
using select_cast_t = typename select_cast<BASE, DERIVED>::type;

template<typename REG, typename... ARGS>
struct for_each_vp;

template<typename REG, typename FIRST, typename... REST>
struct for_each_vp<REG, FIRST, REST...> {

    static void collect_class_info(std::vector<const class_info*>& vp) {
        for_each_vp<REG, REST...>::collect_class_info(vp);
    }

    template<typename SPEC_FIRST, typename... SPEC_REST>
    struct for_spec {
        static void collect_class_info(std::vector<const class_info*>& vp) {
            for_each_vp<REG, REST...>::template for_spec<SPEC_REST...>::collect_class_info(vp);
        }
    };

    enum { count = for_each_vp<REG, REST...>::count };

    static void* resolve(const word* ssp, virtual_base_t<FIRST> first, REST... rest) {
        return for_each_vp<REG, REST...>::resolve(ssp, rest...);
    }
};

template<typename REG, typename FIRST, typename... REST>
struct for_each_vp<REG, virtual_<FIRST>, REST...> {

    static void collect_class_info(std::vector<const class_info*>& vp) {
        vp.push_back(
            &class_info::get<REG, virtual_base_t<virtual_<FIRST>>>());
        for_each_vp<REG, REST...>::collect_class_info(vp);
    }

    template<typename SPEC_FIRST, typename... SPEC_REST>
    struct for_spec {
        static void collect_class_info(std::vector<const class_info*>& vp) {
            vp.push_back(
                &class_info::get<REG, virtual_base_t<virtual_<SPEC_FIRST>>>());
            for_each_vp<REG, REST...>::template for_spec<SPEC_REST...>::collect_class_info(vp);
        }
    };

    enum { count = 1 + for_each_vp<REG, REST...>::count };
};

template<typename REG>
struct for_each_vp<REG> {

    static void collect_class_info(std::vector<const class_info*>& vp) {
    }

    template<typename...>
    struct for_spec {
        static void collect_class_info(std::vector<const class_info*>& vp) {
        }
    };

    enum { count = 0 };
};

template<typename REG, int ARITY, typename... A>
struct resolver;

template<typename REG, typename FIRST, typename... REST>
struct resolver<REG, 1, FIRST, REST...>
{
    static void* resolve(
        const word* ssp,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest) {
        return resolver<REG, 1, REST...>::resolve(ssp, rest...);
    }
};

template<typename REG, typename FIRST, typename... REST>
struct resolver<REG, 1, virtual_<FIRST>, REST...>
{
    static void* resolve(
        const word* ssp,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest) {
        auto key = virtual_traits<virtual_<FIRST>>::key(first);
        _YOMM2_DEBUG(
            detail::log() << "  slot = " << ssp->i << " key = " << key);
        auto mptr = detail::mptr(dispatch_data::instance<REG>, key);
        _YOMM2_DEBUG(detail::log() << " mptr = " << mptr);
        auto pf = mptr[ssp->i].pf;
        _YOMM2_DEBUG(detail::log() << " pf = " << pf << "\n");
        return pf;
    }

    static void* resolve_next(
        const word* ssp,
        const word* dispatch,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest)
    {
        auto key = virtual_traits<virtual_<FIRST>>::key(first);
        _YOMM2_DEBUG(detail::log() << "  key = " << key);
        auto mptr = detail::mptr(dispatch_data::instance<REG>, key);
        _YOMM2_DEBUG(detail::log() << " mptr = " << mptr);
        auto slot = ssp++->i;
        _YOMM2_DEBUG(detail::log() << " slot = " << slot);
        auto stride = ssp++->i;
        _YOMM2_DEBUG(detail::log() << " stride = " << stride);
        dispatch += mptr[slot].i * stride;
        _YOMM2_DEBUG(detail::log() << " dispatch = " << dispatch);
        auto pf = dispatch->pf;
        _YOMM2_DEBUG(detail::log() << " pf = " << pf << "\n");
        return pf;
    }
};

template<typename REG, int ARITY, typename FIRST, typename... REST>
struct resolver<REG, ARITY, FIRST, REST...>
{
    static void* resolve(
        const word* ssp,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest) {
        return resolver<REG, ARITY, REST...>::resolve_first(ssp, rest...);
    }

    static void* resolve_next(
        const word* ssp,
        const word* dispatch,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest)
    {
        return resolver<REG, ARITY, REST...>::resolve_next(ssp, dispatch, rest...);
    }
};

template<typename REG, int ARITY, typename FIRST, typename... REST>
struct resolver<REG, ARITY, virtual_<FIRST>, REST...>
{
    static void* resolve_first(
        const word* ssp,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest)
    {
        auto key = virtual_traits<virtual_<FIRST>>::key(first);
        _YOMM2_DEBUG(detail::log() << "  key = " << key);
        auto mptr = detail::mptr(dispatch_data::instance<REG>, key);
        _YOMM2_DEBUG(detail::log() << " mptr = " << mptr);
        auto slot = ssp++->i;
        _YOMM2_DEBUG(detail::log() << " slot = " << slot);
        auto dispatch = mptr[slot].pw;
        _YOMM2_DEBUG(detail::log() << " dispatch = " << dispatch << "\n");
        return resolver<REG, ARITY - 1, REST...>::resolve_next(
            ssp, dispatch, rest...);
    }

    static void* resolve_next(
        const word* ssp,
        const word* dispatch,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest)
    {
        auto key = virtual_traits<virtual_<FIRST>>::key(first);
        _YOMM2_DEBUG(detail::log() << "  key = " << key);
        auto mptr = detail::mptr(dispatch_data::instance<REG>, key);
        _YOMM2_DEBUG(detail::log() << " mptr = " << mptr);
        auto slot = ssp++->i;
        _YOMM2_DEBUG(detail::log() << " slot = " << slot);
        auto stride = ssp++->i;
        _YOMM2_DEBUG(detail::log() << " stride = " << stride);
        dispatch += mptr[slot].i * stride;
        _YOMM2_DEBUG(detail::log() << " dispatch = " << dispatch << "\n");
        return resolver<REG, ARITY - 1, REST...>::resolve_next(
            ssp, dispatch, rest...);
    }

    static void* resolve(
        const word* ssp,
        virtual_arg_t<FIRST> first,
        virtual_arg_t<REST>... rest)
    {
        return resolve_first(ssp, first, rest...);
    }
};

template<typename BASE_RETURN, class FUNCTION, typename BASE, typename SPEC>
struct wrapper;

template<
    typename BASE_RETURN,
    class FUNCTION,
    typename... BASE_PARAM,
    typename... SPEC_PARAM
    >
struct wrapper<BASE_RETURN, FUNCTION, BASE_RETURN(BASE_PARAM...), BASE_RETURN(SPEC_PARAM...)> {
    static BASE_RETURN body(virtual_arg_t<BASE_PARAM>... arg) {
    return FUNCTION::body(
        virtual_traits<BASE_PARAM>::template cast<SPEC_PARAM>(
            std::forward<virtual_arg_t<BASE_PARAM>>(arg),
            typename select_cast<
                virtual_base_t<BASE_PARAM>,
                virtual_base_t<SPEC_PARAM>>::type())...);
  }
};

template<typename RETURN_T, class METHOD, class SPEC, typename F>
struct register_spec;

template<typename RETURN_T, class METHOD, class SPEC, class... SPEC_ARGS>
struct register_spec<RETURN_T, METHOD, SPEC, void(SPEC_ARGS...)>
{
    register_spec(void** next _YOMM2_COMMA_DEBUG(const char* name)) {
        static spec_info si;
        if (si.vp.empty()) {
            _YOMM2_DEBUG(si.name = name);
            si.pf = (void*) wrapper<
                RETURN_T, SPEC, typename METHOD::signature_t, RETURN_T(SPEC_ARGS...)
                >::body;
            METHOD::for_each_vp_t::template for_spec<SPEC_ARGS...>::collect_class_info(si.vp);
            METHOD::info().specs.push_back(&si);
            si.next = next;
        }
    }
};

template<typename REG, typename ID, typename SIG>
struct method;

template<typename REG, typename ID, typename R, typename... A>
struct method<REG, ID, R(A...)> {

    static const word* slots_strides; // slot 0, slot 1,  stride 1, slot 2, ...

    static method_info& info();

    using signature_t = R(A...);
    using for_each_vp_t = for_each_vp<REG, A...>;

    enum { arity = for_each_vp_t::count };

    static void* resolve(virtual_arg_t<A>... args) {
        _YOMM2_DEBUG(detail::log() << "call " << name() << " slots_strides = " << slots_strides << "\n");
        return resolver<REG, arity, A...>::resolve(slots_strides, args...);
    }

#if YOMM2_DEBUG
    static const char* name() { return info().name; }
#endif

    static void not_implemented(virtual_arg_t<A>...) {
        method_call_error error;
        error.code = method_call_error::not_implemented;
        _YOMM2_DEBUG(error.method_name = info().name);
        detail::call_error_handler(error);
    }

    static void ambiguous(virtual_arg_t<A>...) {
        method_call_error error;
        error.code = method_call_error::ambiguous;
        _YOMM2_DEBUG(error.method_name = info().name);
        detail::call_error_handler(error);
    }

    struct init_method {
        init_method(_YOMM2_DEBUG(const char* name)) {
            if (info().vp.empty()) {
                _YOMM2_DEBUG(info().name = name);
                info().slots_strides_p = &slots_strides;
                for_each_vp_t::collect_class_info(info().vp);
                registry::get<REG>().methods.push_back(&info());
                info().not_implemented = (void*) not_implemented;
                info().ambiguous = (void*) ambiguous;
            }
        }
    };
};

template<typename REG, typename ID, typename R, typename... A>
const word* method<REG, ID, R(A...)>::slots_strides;

template<typename REG, typename ID, typename R, typename... A>
method_info& method<REG, ID, R(A...)>::info() {
    static method_info info;
    return info;
}

void update_methods(const registry& reg, dispatch_data& dd);

} // namespace detail
} // namespace yomm2
} // namespace yorel


#endif
