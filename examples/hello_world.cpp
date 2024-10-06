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

struct Cat : Animal {
};

struct Dog : Animal {
};

BOOST_OPENMETHOD(
    poke, (std::ostream&, virtual_<Animal&>), void);

BOOST_OPENMETHOD_OVERRIDE(
    poke, (std::ostream& os, Cat& cat), void) {
        os << "hiss";
}

BOOST_OPENMETHOD_OVERRIDE(
    poke, (std::ostream& os, Dog& dog), void) {
        os << "bark";
}

BOOST_OPENMETHOD_CLASSES(Animal, Cat, Dog);

struct Bulldog : Dog {
};

BOOST_OPENMETHOD_CLASSES(Dog, Bulldog);

BOOST_OPENMETHOD_OVERRIDE(
    poke, (std::ostream& os, Bulldog& dog), void) {
        next(os, dog); // prints "bark"
        os << " and bite";
}

BOOST_OPENMETHOD(
    encounter, (std::ostream&, virtual_<Animal&>, virtual_<Animal&>), void);

// 'encounter' catch-all implementation.
BOOST_OPENMETHOD_OVERRIDE(encounter, (std::ostream& os, Animal&, Animal&), void) {
    os << "ignore";
}

// Add definitions for specific pairs of animals.
BOOST_OPENMETHOD_OVERRIDE(encounter, (std::ostream& os, Dog& dog1, Dog& dog2), void) {
    os << "wag tail";
}

BOOST_OPENMETHOD_OVERRIDE(encounter, (std::ostream& os, Dog& dog, Cat& cat), void) {
    os << "chase";
}

BOOST_OPENMETHOD_OVERRIDE(encounter, (std::ostream& os, Cat& cat, Dog& dog), void) {
    os << "run";
}

int main() {
    boost::openmethod::initialize();

    std::unique_ptr<Animal> a(new Cat);
    std::unique_ptr<Animal> b(new Dog);

    poke(std::cout, *a); // prints "hiss"
    std::cout << "\n";

    poke(std::cout, *b); // prints "bark"
    std::cout << "\n";

    std::unique_ptr<Animal> c(new Bulldog);
    poke(std::cout, *c); // prints "bark and bite"
    std::cout << "\n";

    encounter(std::cout, *a, *b); // prints "run"
    std::cout << "\n";

    return 0;
}

void call_poke(std::ostream& os, Animal& a) {
    poke(os, a);
}

void call_encounter(std::ostream& os, Animal& a, Animal& b) {
    encounter(os, a, b);
}
