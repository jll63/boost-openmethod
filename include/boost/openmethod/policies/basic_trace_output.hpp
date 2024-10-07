
// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_OPENMETHOD_POLICY_BASIC_TRACE_OUTPUT_HPP
#define BOOST_OPENMETHOD_POLICY_BASIC_TRACE_OUTPUT_HPP

#include <boost/openmethod/policies/core.hpp>
#include <boost/openmethod/detail/ostdstream.hpp>

namespace boost {
namespace openmethod {
namespace policies {

template<class Policy, typename Stream = detail::ostderr>
struct basic_trace_output : virtual trace_output {
    static Stream trace_stream;
    static bool trace_enabled;
};

template<class Policy, typename Stream>
Stream basic_trace_output<Policy, Stream>::trace_stream;

template<class Policy, typename Stream>
bool basic_trace_output<Policy, Stream>::trace_enabled([]() {
    auto env = getenv("BOOST_OPENMETHOD_TRACE");
    return env && *env++ == '1' && *env++ == 0;
}());

} // namespace policies
} // namespace openmethod
} // namespace boost

#endif
