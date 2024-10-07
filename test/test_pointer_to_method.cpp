// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/openmethod.hpp>
#include <boost/openmethod/compiler.hpp>

#include <string>

#define BOOST_TEST_MODULE openmethod
#include <boost/test/included/unit_test.hpp>

using boost::openmethod::virtual_;

class Animal {
  public:
    virtual ~Animal() {
    }
};

class Dog : public Animal {};

BOOST_OPENMETHOD_CLASSES(Animal, Dog, Animal);

BOOST_OPENMETHOD(poke, (virtual_<Animal&>), std::string);

BOOST_OPENMETHOD_OVERRIDE(poke, (Dog & dog), std::string) {
    return "bark";
}

BOOST_AUTO_TEST_CASE(noadl) {
    boost::openmethod::initialize();
    auto stimulus = &poke;
    Dog snoopy;
    Animal& animal = snoopy;
    BOOST_TEST(stimulus(snoopy) == "bark");
}
