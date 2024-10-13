// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

struct custom_type_info {
    static unsigned last;
    unsigned id = ++last;
};

unsigned custom_type_info::last;

struct Animal {
    Animal() {
        type = type_info.id;
    }

    virtual ~Animal() = default;

    virtual void* cast_impl(unsigned target) {
        if (type_info.id == target) {
            return this;
        } else {
            return nullptr;
        }
    }

    template<class Class>
    Class* cast() {
        return reinterpret_cast<Class*>(cast_impl(Class::type_info.id));
    }

    static custom_type_info type_info;
    unsigned type;
};

custom_type_info Animal::type_info;

struct Cat : virtual Animal {
    Cat() {
        type = type_info.id;
    }

    virtual void* cast_impl(unsigned target) {
        if (type_info.id == target) {
            return this;
        } else {
            return Animal::cast_impl(target);
        }
    }

    static custom_type_info type_info;
};

custom_type_info Cat::type_info;

struct Dog : virtual Animal {
    Dog() {
        type = type_info.id;
    }

    virtual void* cast_impl(unsigned target) {
        if (type_info.id == target) {
            return this;
        } else {
            return Animal::cast_impl(target);
        }
    }

    static custom_type_info type_info;
};

// put Dog's type_info *after* open-method static ctors to test the
// deferred_static_rtti policy

#include <boost/openmethod/policies/core.hpp>
#include <boost/openmethod/policies/vptr_vector.hpp>

namespace bom = boost::openmethod;

struct custom_rtti : bom::policies::rtti {
    template<typename T>
    static bom::type_id static_type() {
        if constexpr (std::is_base_of_v<Animal, T>) {
            return T::type_info.id;
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

    template<typename Derived, typename Base>
    static Derived dynamic_cast_ref(Base&& obj) {
        using base_type = std::remove_reference_t<Base>;
        if constexpr (std::is_base_of_v<Animal, base_type>) {
            return *obj.template cast<std::remove_reference_t<Derived>>();
        } else {
            abort(); // not supported
        }
    }
};

struct custom_policy
    : bom::policies::basic_policy<
          custom_policy, custom_rtti, bom::policies::deferred_static_rtti,
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

custom_type_info Dog::type_info;

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
