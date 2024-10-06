
// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_OPENMETHOD_POLICY_BASIC_INDIRECT_VPTR_HPP
#define BOOST_OPENMETHOD_POLICY_BASIC_INDIRECT_VPTR_HPP

#include <boost/openmethod/policies/core.hpp>

#include <vector>

namespace boost {
namespace openmethod {
namespace policies {

template<class Policy>
struct basic_indirect_vptr : virtual indirect_vptr {
    static std::vector<std::uintptr_t const* const*> indirect_vptrs;
};

template<class Policy>
std::vector<std::uintptr_t const* const*>
    basic_indirect_vptr<Policy>::indirect_vptrs;

} // namespace policies
} // namespace openmethod
} // namespace boost

#endif
