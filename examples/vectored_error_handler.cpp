// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/openmethod.hpp>
#include <boost/openmethod/compiler.hpp>

struct Animal {
    virtual ~Animal() = default;
};

struct Cat : Animal {};
struct Dog : Animal {};

BOOST_OPENMETHOD_CLASSES(Animal, Cat, Dog);

BOOST_OPENMETHOD(trick, (std::ostream&, virtual_<Animal&>), void);

BOOST_OPENMETHOD_OVERRIDE(trick, (std::ostream & os, Dog& dog), void) {
    os << "spin\n";
}

int main() {
    namespace bom = boost::openmethod;
    bom::initialize();

    bom::policies::default_::set_error_handler([](const auto& error) {
        std::visit([](auto&& arg) { throw arg; }, error);
    });

    Cat felix;
    Dog hector, snoopy;
    std::vector<Animal*> animals = {&hector, &felix, &snoopy};

    for (auto animal : animals) {
        try {
            trick(std::cout, *animal);
        } catch (bom::not_implemented_error) {
            std::cerr << boost::core::demangle(typeid(*animal).name())
                      << "s cannot perform tricks\n";
        }
    }

    return 0;
}
