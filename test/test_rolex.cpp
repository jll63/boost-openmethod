#include <boost/openmethod.hpp>
#include <boost/openmethod/compiler.hpp>

struct Role {
    virtual ~Role() {
    }
};

struct Employee : Role {
    virtual double pay() const;
};

struct Manager : Employee {
    virtual double pay() const;
};

struct Founder : Role {};

struct Expense {
    virtual ~Expense() {
    }
};

struct Public : Expense {};
struct Bus : Public {};
struct Metro : Public {};
struct Taxi : Expense {};
struct Jet : Expense {};

BOOST_OPENMETHOD_CLASSES(
    Role, Employee, Manager, Founder, Expense, Public, Bus, Metro, Taxi, Jet);

BOOST_OPENMETHOD(pay, (virtual_<const Employee&>), double);
BOOST_OPENMETHOD(
    approve, (virtual_<const Role&>, virtual_<const Expense&>, double), bool);

BOOST_OPENMETHOD_OVERRIDE(pay, (const Employee&), double) {
    return 3000;
}

BOOST_OPENMETHOD_OVERRIDE(pay, (const Manager& exec), double) {
    return next(exec) + 2000;
}

BOOST_OPENMETHOD_OVERRIDE(
    approve, (const Role& r, const Expense& e, double amount), bool) {
    return false;
}

BOOST_OPENMETHOD_OVERRIDE(
    approve, (const Employee& r, const Public& e, double amount), bool) {
    return true;
}

BOOST_OPENMETHOD_OVERRIDE(
    approve, (const Manager& r, const Taxi& e, double amount), bool) {
    return true;
}

BOOST_OPENMETHOD_OVERRIDE(
    approve, (const Founder& r, const Expense& e, double amount), bool) {
    return true;
}

int main() {
    boost::openmethod::initialize();
}

double call_pay(const Employee& emp) {
    return pay(emp);
}

double Employee::pay() const {
    return 3000;
}

double Manager::pay() const {
    return Employee::pay() + 2000;
}

double call_pay_vfunc(const Employee& emp) {
    return emp.pay();
}

bool call_approve(const Role& r, const Expense& e, double a) {
    return approve(r, e, a);
}
