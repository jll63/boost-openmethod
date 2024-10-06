#include <boost/openmethod.hpp>
#include <boost/openmethod/compiler.hpp>

#include "test_util.hpp"

#define BOOST_TEST_MODULE openmethod
#include <boost/test/included/unit_test.hpp>
#include <boost/utility/identity_type.hpp>

enum Status { ORIGINAL, COPY, MOVED, DEAD };

struct Base {
    int id;
    Status status;
    Base(int id) : id(id), status(ORIGINAL) {
    }
    Base(const Base& other) : id(other.id), status(COPY) {
    }
    Base(Base&& other) : id(other.id), status(MOVED) {
        other.status = DEAD;
    }
    virtual ~Base() {
    }
};

struct Derived : Base {
    using Base::Base;
};

// -----------------------------------------------------------------------------
// Test test apparatus.

BOOST_AUTO_TEST_CASE_TEMPLATE(
    test_moveable, Class, BOOST_IDENTITY_TYPE((std::tuple<Base, Derived>))) {
    Class a(1);

    BOOST_REQUIRE(a.status == ORIGINAL);

    Class b = a;
    BOOST_REQUIRE(a.status == ORIGINAL);
    BOOST_REQUIRE(b.status == COPY);

    Class c = std::move(a);
    BOOST_REQUIRE(a.status == DEAD);
    BOOST_REQUIRE(c.status == MOVED);
}

// -----------------------------------------------------------------------------

BOOST_OPENMETHOD(move_non_virtual_arg, (virtual_<Base&>, Base&&), void);

BOOST_OPENMETHOD_OVERRIDE(
    move_non_virtual_arg, (Base & ref_arg, Base&& moving_arg), void) {
    BOOST_TEST(ref_arg.status == ORIGINAL);
    BOOST_TEST(moving_arg.status == ORIGINAL);
    Base moved = std::move(moving_arg);
}

BOOST_AUTO_TEST_CASE(test_move_non_virtual_arg) {
    Base ref_arg(1), moving_arg(2);
    move_non_virtual_arg(ref_arg, std::move(moving_arg));
    BOOST_TEST(moving_arg.status == DEAD);
}

// -----------------------------------------------------------------------------

BOOST_OPENMETHOD(move_virtual_arg, (Base&, virtual_<Base&&>), void);

BOOST_OPENMETHOD_OVERRIDE(
    move_virtual_arg, (Base & ref_arg, Base&& moving_arg), void) {
    BOOST_TEST(ref_arg.status == ORIGINAL);
    BOOST_TEST(moving_arg.status == ORIGINAL);
    Base moved = std::move(moving_arg);
}

BOOST_AUTO_TEST_CASE(test_move_virtual_arg) {
    Base ref_arg(1), moving_arg(2);
    move_virtual_arg(ref_arg, std::move(moving_arg));
    BOOST_TEST(moving_arg.status == DEAD);
}

// -----------------------------------------------------------------------------
// Global initialization.

BOOST_OPENMETHOD_CLASSES(Base, Derived);

struct openmethod_update {
    openmethod_update() {
        boost::openmethod::initialize();
    }
};

BOOST_TEST_GLOBAL_FIXTURE(openmethod_update);
