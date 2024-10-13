// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_OPENMETHOD_POLICIES_CORE_HPP
#define BOOST_OPENMETHOD_POLICIES_CORE_HPP

#include <boost/openmethod/detail/types.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>

#include <functional>
#include <variant>

namespace boost {
namespace openmethod {
namespace detail {

using class_catalog = detail::static_list<detail::class_info>;
using method_catalog = detail::static_list<detail::method_info>;

} // namespace detail

// -----------------------------------------------------------------------------
// Error handling

struct error {};

struct not_implemented_error : error {
    type_id method;
    std::size_t arity;
    static constexpr std::size_t max_types = 16;
    type_id types[max_types];
};

struct unknown_class_error : error {
    enum { update = 1, call } context;
    type_id type;
};

struct hash_search_error : error {
    std::size_t attempts;
    std::size_t buckets;
};

struct method_table_error : error {
    type_id type;
};

struct static_offset_error : error {
    type_id method;
    int actual, expected;
};

struct static_slot_error : static_offset_error {};
struct static_stride_error : static_offset_error {};

namespace policies {

struct abstract_policy {};

// -----------------------------------------------------------------------------
// Facets

struct rtti {
    static auto type_index(type_id type) -> type_id {
        return type;
    }

    template<typename Stream>
    static void type_name(type_id type, Stream& stream) {
        stream << "type_id(" << type << ")";
    }
};

struct deferred_static_rtti : virtual rtti {};

struct error_handler {};
struct indirect_vptr {};
struct type_hash {};
struct vptr {};
struct external_vptr : virtual vptr {};
struct error_output {};
struct trace_output {};
struct runtime_checks {};

// -----------------------------------------------------------------------------
// Facet implementations

struct deferred_static_rtti;
struct debug;
struct release;

// -----------------------------------------------------------------------------
// basic_domain

template<class Name>
struct basic_domain {
    static detail::class_catalog classes;
    static detail::method_catalog methods;
    template<class Class>
    static std::uintptr_t* static_vptr;
    static std::vector<std::uintptr_t> dispatch_data;
};

template<class Name>
detail::class_catalog basic_domain<Name>::classes;

template<class Name>
detail::method_catalog basic_domain<Name>::methods;

template<class Name>
template<class Class>
std::uintptr_t* basic_domain<Name>::static_vptr;

template<class Name>
std::vector<std::uintptr_t> basic_domain<Name>::dispatch_data;

template<typename Policy, class Facet>
struct rebind_facet {
    using type = Facet;
};

template<
    typename NewPolicy, typename OldPolicy,
    template<typename...> class GenericFacet, typename... Args>
struct rebind_facet<NewPolicy, GenericFacet<OldPolicy, Args...>> {
    using type = GenericFacet<NewPolicy, Args...>;
};

template<class Policy, class... Facets>
struct basic_policy : virtual abstract_policy,
                      virtual basic_domain<Policy>,
                      virtual Facets... {
    using facets = detail::types<Facets...>;

    template<class Facet>
    static constexpr bool has_facet = std::is_base_of_v<Facet, basic_policy>;

    template<class Facet>
    using use_facet = boost::mp11::mp_first<boost::mp11::mp_filter_q<
        boost::mp11::mp_bind_front_q<
            boost::mp11::mp_quote_trait<std::is_base_of>, Facet>,
        facets>>;

    template<class NewPolicy>
    using rebind = basic_policy<
        NewPolicy, typename rebind_facet<NewPolicy, Facets>::type...>;

    template<class... MoreFacets>
    using add = basic_policy<basic_policy, Facets..., MoreFacets...>;

    template<class Base, class Facet>
    using replace = boost::mp11::mp_apply<
        basic_policy,
        boost::mp11::mp_push_front<
            boost::mp11::mp_replace_if_q<
                facets,
                boost::mp11::mp_bind_front_q<
                    boost::mp11::mp_quote_trait<std::is_base_of>, Base>,
                Facet>,
            Policy>>;

    template<class Base>
    using remove = boost::mp11::mp_apply<
        basic_policy,
        boost::mp11::mp_push_front<
            boost::mp11::mp_remove_if_q<
                facets,
                boost::mp11::mp_bind_front_q<
                    boost::mp11::mp_quote_trait<std::is_base_of>, Base>>,
            Policy>>;
};

} // namespace policies

} // namespace openmethod
} // namespace boost

#endif
