// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/openmethod/compiler.hpp>
#include <boost/openmethod/core.hpp>
#include <boost/openmethod/macros/name.hpp>
#include <boost/openmethod/macros/register.hpp>

struct Animal {
    virtual ~Animal() = default;
};

struct Cat : Animal {};

struct Dog : Animal {};

struct Bulldog : Dog {};

using namespace boost::openmethod;

class poke_openmethod;

using poke = method<poke_openmethod(std::ostream&, virtual_<Animal&>), void>;

auto poke_cat(std::ostream& os, Cat& cat) {
    os << "hiss";
}

static poke::override<poke_cat> override_poke_cat;

auto poke_dog(std::ostream& os, Dog& dog) {
    os << "bark";
}

BOOST_OPENMETHOD_REGISTER(poke::override<poke_dog>);

auto poke_bulldog(std::ostream& os, Bulldog& dog) -> void {
    poke::next<poke_bulldog>(os, dog);
    os << " and bite";
}

BOOST_OPENMETHOD_REGISTER(poke::override<poke_bulldog>);

class BOOST_OPENMETHOD_NAME(pet);

auto pet_cat(std::ostream& os, Cat& cat) {
    os << "purr";
}

auto pet_dog(std::ostream& os, Dog& dog) {
    os << "wag tail";
}

using pet =
    method<BOOST_OPENMETHOD_NAME(pet)(std::ostream&, virtual_<Animal&>), void>;

BOOST_OPENMETHOD_REGISTER(pet::override<pet_cat, pet_dog>);

BOOST_OPENMETHOD_REGISTER(use_classes<Animal, Cat, Dog, Bulldog>);

int main() {
    boost::openmethod::initialize();

    std::unique_ptr<Animal> a(new Cat);
    std::unique_ptr<Animal> b(new Dog);

    poke::fn(std::cout, *a); // prints "hiss"
    std::cout << "\n";

    poke::fn(std::cout, *b); // prints "bark"
    std::cout << "\n";

    std::unique_ptr<Animal> c(new Bulldog);
    poke::fn(std::cout, *c); // prints "bark and bite"
    std::cout << "\n";

    pet::fn(std::cout, *a); // prints "purr"
    std::cout << "\n";

    return 0;
}
