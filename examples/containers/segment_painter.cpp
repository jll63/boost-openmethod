// Copyright (c) 2021 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// This exmaple is based on sample code provided by Github user matpen in
// https://github.com/jll63/yomm2/issues/7

#include <iostream>

#include <yorel/yomm2.hpp>

#include "geometries.hpp"
#include "painter.hpp"
#include "line_painter.hpp"

register_classes(geometries::Segment, geometries::Line);

namespace painter {
namespace paint1d {

define_method_in(
    painters, paintObject,
    (Painter & painter, const geometries::Segment& segment), void) {
    ++painter.counter;
    painters<YOMM2_METHOD_NAME(paintObject)(Painter&, const geometries::Line&)>::fn(
        painter, segment);
    std::cout << " " << "painting segment\n";
}

} // namespace paint1d
} // namespace painter
