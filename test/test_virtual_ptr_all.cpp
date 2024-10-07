// Copyright (c) 2018-2024 Jean-Louis Leroy
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/openmethod.hpp>
#include <boost/openmethod/compiler.hpp>
#include <boost/openmethod/templates.hpp>
#include <boost/openmethod/virtual_shared_ptr.hpp>

#include "test_util.hpp"

#define BOOST_TEST_MODULE openmethod
#include <boost/test/included/unit_test.hpp>
#include <boost/utility/identity_type.hpp>

using namespace boost::openmethod;
using boost::mp11::mp_list;

namespace boost {
namespace openmethod {
struct direct {};
struct indirect {};
} // namespace openmethod
} // namespace boost

struct Player {
    virtual ~Player() {
    }
};

struct Warrior : Player {};
struct Wizard : Player {};

struct Bear : Player {};

struct Object {
    virtual ~Object() {
    }
};

struct Axe : Object {};

template<class VirtualBearPtr>
auto poke_bear(VirtualBearPtr) {
    return std::string("growl");
}

template<class VirtualWarriorPtr, class VirtualAxePtr, class VirtualBearPtr>
auto fight_bear(VirtualWarriorPtr, VirtualAxePtr, VirtualBearPtr) {
    return "kill bear";
}

template<int Name>
struct indirect_test_policy : test_policy_<Name> {
    static constexpr bool use_indirect_method_pointers = true;
};

template<int Name>
using policy_types = types<test_policy_<Name>, indirect_test_policy<Name>>;

struct BOOST_OPENMETHOD_NAME(poke);
struct BOOST_OPENMETHOD_NAME(fight);

namespace BOOST_OPENMETHOD_GENSYM {

BOOST_AUTO_TEST_CASE_TEMPLATE(
    test_virtual_ptr, Policy, policy_types<__COUNTER__>) {
    using namespace detail;

    static use_classes<Player, Warrior, Object, Axe, Bear, Policy>
        BOOST_OPENMETHOD_GENSYM;
    using poke = method<
        BOOST_OPENMETHOD_NAME(poke)(virtual_ptr<Player, Policy>), std::string,
        Policy>;
    static
        typename poke::template override<poke_bear<virtual_ptr<Player, Policy>>>
            BOOST_OPENMETHOD_GENSYM;

    initialize<Policy>();

    using vptr_player = virtual_ptr<Player, Policy>;
    static_assert(is_virtual_ptr<vptr_player>);
    using vptr_cat = virtual_ptr<Bear, Policy>;

    Player player;
    auto virtual_player = vptr_player::final(player);
    BOOST_TEST(&*virtual_player == &player);
    BOOST_TEST(
        (virtual_player._vptr() == Policy::template static_vptr<Player>));

    Bear bear;
    BOOST_TEST((&*vptr_cat::final(bear)) == &bear);
    BOOST_TEST(
        (vptr_cat::final(bear)._vptr() == Policy::template static_vptr<Bear>));

    BOOST_TEST(
        (vptr_player(bear)._vptr() == Policy::template static_vptr<Bear>));

    vptr_cat virtual_cat_ptr(bear);
    vptr_player virtual_player_ptr = virtual_cat_ptr;

    struct upcast {
        static void fn(vptr_player) {
        }
    };

    upcast::fn(virtual_cat_ptr);

    auto data = Policy::dispatch_data.data();

    while (data == Policy::dispatch_data.data()) {
        Policy::dispatch_data.resize(2 * Policy::dispatch_data.size());
    }

    initialize<Policy>();

    BOOST_TEST(
        (virtual_cat_ptr._vptr() == Policy::template static_vptr<Bear>) ==
        Policy::template has_facet<policies::indirect_vptr>);
}
} // namespace BOOST_OPENMETHOD_GENSYM

namespace test_virtual_ptr_dispatch {

BOOST_AUTO_TEST_CASE_TEMPLATE(
    test_virtual_ptr_dispatch, Policy, policy_types<__COUNTER__>) {

    static use_classes<Player, Warrior, Object, Axe, Bear, Policy>
        BOOST_OPENMETHOD_GENSYM;

    using poke = method<
        BOOST_OPENMETHOD_NAME(poke)(virtual_ptr<Player, Policy>), std::string,
        Policy>;
    static
        typename poke::template override<poke_bear<virtual_ptr<Player, Policy>>>
            BOOST_OPENMETHOD_GENSYM;

    using fight = method<
        BOOST_OPENMETHOD_NAME(fight)(
            virtual_ptr<Player, Policy>, virtual_ptr<Object, Policy>,
            virtual_ptr<Player, Policy>),
        std::string, Policy>;
    static typename fight::template override<fight_bear<
        virtual_ptr<Player, Policy>, virtual_ptr<Object, Policy>,
        virtual_ptr<Player, Policy>>>
        BOOST_OPENMETHOD_GENSYM;

    initialize<Policy>();

    Bear bear;
    BOOST_TEST(poke::fn(virtual_ptr<Player, Policy>(bear)) == "growl");

    Warrior warrior;
    Axe axe;
    BOOST_TEST(
        fight::fn(
            virtual_ptr<Player, Policy>(warrior),
            virtual_ptr<Object, Policy>(axe),
            virtual_ptr<Player, Policy>(bear)) == "kill bear");
}

} // namespace test_virtual_ptr_dispatch

namespace test_virtual_shared_ptr {

BOOST_AUTO_TEST_CASE(test_virtual_shared_ptr) {
    auto bear = std::make_shared<Bear>();
    virtual_shared_ptr<Bear> vbear(bear);
}

} // namespace test_virtual_shared_ptr

namespace test_virtual_shared_ptr_dispatch {

BOOST_AUTO_TEST_CASE_TEMPLATE(
    test_virtual_ptr_dispatch, Policy, policy_types<__COUNTER__>) {

    static use_classes<Player, Warrior, Object, Axe, Bear, Policy>
        BOOST_OPENMETHOD_GENSYM;

    using poke = method<
        BOOST_OPENMETHOD_NAME(poke)(virtual_shared_ptr<Player, Policy>),
        std::string, Policy>;

    static typename poke::template override<
        poke_bear<virtual_shared_ptr<Player, Policy>>>
        BOOST_OPENMETHOD_GENSYM;

    using fight = method<
        BOOST_OPENMETHOD_NAME(fight)(
            virtual_shared_ptr<Player, Policy>,
            virtual_shared_ptr<Object, Policy>,
            virtual_shared_ptr<Player, Policy>),
        std::string, Policy>;

    static typename fight::template override<fight_bear<
        virtual_shared_ptr<Player, Policy>, virtual_shared_ptr<Object, Policy>,
        virtual_shared_ptr<Player, Policy>>>
        BOOST_OPENMETHOD_GENSYM;

    initialize<Policy>();

    auto bear = make_virtual_shared<Bear, Policy>();
    auto warrior = make_virtual_shared<Warrior, Policy>();
    auto axe = make_virtual_shared<Axe, Policy>();

    BOOST_TEST(poke::fn(bear) == "growl");

    BOOST_TEST(fight::fn(warrior, axe, bear) == "kill bear");
}

} // namespace test_virtual_shared_ptr_dispatch
