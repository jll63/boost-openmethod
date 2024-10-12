// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_OPENMETHOD_POLICY_HPP
#define BOOST_OPENMETHOD_POLICY_HPP

#include <functional>
#include <memory>
#include <variant>
#include <vector>

#include <boost/openmethod/policies/core.hpp>

#include <boost/openmethod/policies/minimal_rtti.hpp>
#include <boost/openmethod/policies/std_rtti.hpp>
#include <boost/openmethod/policies/vptr_vector.hpp>
#include <boost/openmethod/policies/vptr_map.hpp>
#include <boost/openmethod/policies/basic_indirect_vptr.hpp>
#include <boost/openmethod/policies/basic_error_output.hpp>
#include <boost/openmethod/policies/basic_trace_output.hpp>
#include <boost/openmethod/policies/fast_perfect_hash.hpp>
#include <boost/openmethod/policies/vectored_error_handler.hpp>

#ifndef BOOST_NO_EXCEPTIONS
#include <boost/openmethod/policies/throw_error.hpp>
#endif

namespace boost {
namespace openmethod {
namespace policies {

struct release : basic_policy<
                     release, std_rtti, fast_perfect_hash<release>,
                     vptr_vector<release>, vectored_error_handler<release>> {};

struct debug : release::rebind<debug>::add<
                   runtime_checks, basic_error_output<debug>,
                   basic_trace_output<debug>> {};

static_assert(std::is_base_of_v<trace_output, debug>);
static_assert(debug::has_facet<trace_output>);

#ifdef NDEBUG
using default_ = policies::release;
#else
using default_ = policies::debug;
#endif

} // namespace policies

} // namespace openmethod
} // namespace boost

#endif
