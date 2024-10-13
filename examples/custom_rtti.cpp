// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

struct Animal {
    Animal(unsigned type) : type(type) {
    }

    virtual ~Animal() = default;

    unsigned type;
    static constexpr unsigned static_type = 1;
};

struct Cat : Animal {
    Cat() : Animal(static_type) {
    }

    static constexpr unsigned static_type = 2;
};

struct Dog : Animal {
    Dog() : Animal(static_type) {
    }

    static constexpr unsigned static_type = 3;
};

#include <boost/openmethod/policies/core.hpp>
#include <boost/openmethod/policies/vptr_vector.hpp>

namespace bom = boost::openmethod;

struct custom_rtti : bom::policies::rtti {
    template<typename T>
    static bom::type_id static_type() {
        if constexpr (std::is_base_of_v<Animal, T>) {
            return T::static_type;
        } else {
            return 0;
        }
    }

    template<typename T>
    static bom::type_id dynamic_type(const T& obj) {
        if constexpr (std::is_base_of_v<Animal, T>) {
            return obj.type;
        } else {
            return 0;
        }
    }
};

struct custom_policy
    : bom::policies::basic_policy<
          custom_policy, custom_rtti,
          bom::policies::vptr_vector<custom_policy>> {};

#define BOOST_OPENMETHOD_DEFAULT_POLICY custom_policy

#include <iostream>

#include <boost/openmethod.hpp>
#include <boost/openmethod/compiler.hpp>

BOOST_OPENMETHOD(poke, (std::ostream&, virtual_<Animal&>), void);

BOOST_OPENMETHOD_OVERRIDE(poke, (std::ostream & os, Cat& cat), void) {
    os << "hiss";
}

BOOST_OPENMETHOD_OVERRIDE(poke, (std::ostream & os, Dog& dog), void) {
    os << "bark";
}

BOOST_OPENMETHOD_CLASSES(Animal, Cat, Dog);

int main() {
    boost::openmethod::initialize();

    std::unique_ptr<Animal> a(new Cat);
    std::unique_ptr<Animal> b(new Dog);

    poke(std::cout, *a); // prints "hiss"
    std::cout << "\n";

    poke(std::cout, *b); // prints "bark"
    std::cout << "\n";

    return 0;
}
