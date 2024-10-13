
// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_OPENMETHOD_POLICY_VECTORED_ERROR_HPP
#define BOOST_OPENMETHOD_POLICY_VECTORED_ERROR_HPP

#include <boost/openmethod/policies/core.hpp>

#include <functional>
#include <variant>

namespace boost {
namespace openmethod {
namespace policies {

template<class Policy>
class vectored_error_handler : public virtual error_handler {
  public:
    using error_variant = std::variant<
        error, not_implemented_error, unknown_class_error, hash_search_error,
        method_table_error, static_slot_error, static_stride_error>;

    using error_handler_type = std::function<void(const error_variant& error)>;

    template<class Error>
    static auto error(const Error& error) {
        fn(error_variant(error));
    }

    static auto set_error_handler(error_handler_type handler) {
        auto prev = fn;
        fn = handler;

        return prev;
    }

  private:
    static error_handler_type fn;

    static auto default_handler(const error_variant& error_v) {
        using namespace detail;
        using namespace policies;

        if constexpr (Policy::template has_facet<error_output>) {
            if (auto error = std::get_if<not_implemented_error>(&error_v)) {
                Policy::error_stream << "no applicable overrider for ";
                Policy::type_name(error->method, Policy::error_stream);
                Policy::error_stream << "(";
                auto comma = "";

                for (auto ti :
                     range{error->types, error->types + error->arity}) {
                    Policy::error_stream << comma;
                    Policy::type_name(ti, Policy::error_stream);
                    comma = ", ";
                }

                Policy::error_stream << ")\n";
            } else if (
                auto error = std::get_if<unknown_class_error>(&error_v)) {
                Policy::error_stream << "unknown class ";
                Policy::type_name(error->type, Policy::error_stream);
                Policy::error_stream << "\n";
            } else if (auto error = std::get_if<method_table_error>(&error_v)) {
                Policy::error_stream << "invalid method table for ";
                Policy::type_name(error->type, Policy::error_stream);
                Policy::error_stream << "\n";
            } else if (auto error = std::get_if<hash_search_error>(&error_v)) {
                Policy::error_stream << "could not find hash factors after "
                                     << error->attempts << "s using "
                                     << error->buckets << " buckets\n";
            }
        }
    }
};

template<class Policy>
typename vectored_error_handler<Policy>::error_handler_type
    vectored_error_handler<Policy>::fn =
        vectored_error_handler<Policy>::default_handler;

} // namespace policies
} // namespace openmethod
} // namespace boost

#endif
