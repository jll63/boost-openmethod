// dl_shared.cpp
// Copyright (c) 2018-2021 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>

#include <yorel/yomm2.hpp>
#include <yorel/yomm2/compiler.hpp>

#include "dl.hpp"

using namespace std;

define_method(encounter, (const Herbivore&, const Carnivore&), string) {
    return "run";
}

struct Tiger : Carnivore {};

register_classes(Tiger, Carnivore);

extern "C" Tiger* make_tiger() {
    return new Tiger;
}

define_method(encounter, (const Carnivore&, const Herbivore&), string) {
    return "hunt";
}
