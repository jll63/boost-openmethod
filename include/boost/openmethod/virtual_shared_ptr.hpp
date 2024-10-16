#ifndef BOOST_OPENMETHOD_VIRTUAL_SHARED_PTR_HPP
#define BOOST_OPENMETHOD_VIRTUAL_SHARED_PTR_HPP

#include <boost/openmethod/core.hpp>

#include <memory>

namespace boost {
namespace openmethod {

namespace detail {

template<typename T>
struct shared_ptr_traits {
    static const bool is_shared_ptr = false;
};

template<typename T>
struct shared_ptr_traits<std::shared_ptr<T>> {
    static const bool is_shared_ptr = true;
    static const bool is_const_ref = false;
    using polymorphic_type = T;
};

template<typename T>
struct shared_ptr_traits<const std::shared_ptr<T>&> {
    static const bool is_shared_ptr = true;
    static const bool is_const_ref = true;
    using polymorphic_type = T;
};

template<class Policy, typename T>
struct virtual_traits<Policy, const std::shared_ptr<T>&> {
    using polymorphic_type = std::remove_cv_t<T>;

    static auto rarg(const std::shared_ptr<T>& arg) -> const T& {
        return *arg;
    }

    template<class DERIVED>
    static void check_cast() {
        static_assert(shared_ptr_traits<DERIVED>::is_shared_ptr);
        static_assert(
            shared_ptr_traits<DERIVED>::is_const_ref,
            "cannot cast from 'const shared_ptr<base>&' to "
            "'shared_ptr<derived>'");
        static_assert(std::is_class_v<
                      typename shared_ptr_traits<DERIVED>::polymorphic_type>);
    }

    template<class DERIVED>
    static auto cast(const std::shared_ptr<T>& obj) {
        check_cast<DERIVED>();

        if constexpr (detail::requires_dynamic_cast<T*, DERIVED>) {
            return std::dynamic_pointer_cast<
                typename shared_ptr_traits<DERIVED>::polymorphic_type>(obj);
        } else {
            return std::static_pointer_cast<
                typename shared_ptr_traits<DERIVED>::polymorphic_type>(obj);
        }
    }
};

template<class Policy, typename T>
struct virtual_traits<Policy, std::shared_ptr<T>> {
    using polymorphic_type = std::remove_cv_t<T>;

    static auto rarg(const std::shared_ptr<T>& arg) -> const T& {
        return *arg;
    }

    template<class DERIVED>
    static void check_cast() {
        static_assert(shared_ptr_traits<DERIVED>::is_shared_ptr);
        static_assert(
            !shared_ptr_traits<DERIVED>::is_const_ref,
            "cannot cast from 'const shared_ptr<base>&' to "
            "'shared_ptr<derived>'");
        static_assert(std::is_class_v<
                      typename shared_ptr_traits<DERIVED>::polymorphic_type>);
    }
    template<class DERIVED>
    static auto cast(const std::shared_ptr<T>& obj) {
        check_cast<DERIVED>();

        if constexpr (detail::requires_dynamic_cast<T*, DERIVED>) {
            return std::dynamic_pointer_cast<
                typename shared_ptr_traits<DERIVED>::polymorphic_type>(obj);
        } else {
            return std::static_pointer_cast<
                typename shared_ptr_traits<DERIVED>::polymorphic_type>(obj);
        }
    }
};

template<class Class, class Policy>
struct virtual_ptr_traits<std::shared_ptr<Class>, Policy> {
    static bool constexpr is_smart_ptr = true;
    using polymorphic_type = Class;

    template<typename OtherPtrRef>
    static auto cast(const std::shared_ptr<Class>& ptr) -> decltype(auto) {
        using OtherPtr = typename std::remove_reference_t<OtherPtrRef>;
        using OtherClass = typename OtherPtr::pointer_type::element_type;

        if constexpr (detail::requires_dynamic_cast<Class&, OtherClass&>) {
            return std::dynamic_pointer_cast<OtherClass>(ptr);
        } else {
            return std::static_pointer_cast<OtherClass>(ptr);
        }
    }
};

template<class... Ts>
using virtual_ptr_class = std::conditional_t<
    sizeof...(Ts) == 2, boost::mp11::mp_second<detail::types<Ts..., void>>,
    boost::mp11::mp_first<detail::types<Ts...>>>;

} // namespace detail

template<class Class, class Policy = BOOST_OPENMETHOD_DEFAULT_POLICY>
using virtual_shared_ptr = virtual_ptr<std::shared_ptr<Class>, Policy>;

template<class Class, class Policy = BOOST_OPENMETHOD_DEFAULT_POLICY>
inline auto make_virtual_shared() {
    return virtual_shared_ptr<Class, Policy>::final(
        std::make_shared<detail::virtual_ptr_class<Class>>());
}

} // namespace openmethod
} // namespace boost

#endif
