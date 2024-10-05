#include <yorel/yomm2.hpp>
#include <yorel/yomm2/compiler.hpp>

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

register_classes(
    Role, Employee, Manager, Founder, Expense, Public, Bus, Metro, Taxi, Jet);

declare_method(pay, (virtual_<const Employee&>), double);
declare_method(
    approve, (virtual_<const Role&>, virtual_<const Expense&>, double), bool);

define_method(pay, (const Employee&), double) {
    return 3000;
}

define_method(pay, (const Manager& exec), double) {
    return next(exec) + 2000;
}

define_method(approve, (const Role& r, const Expense& e, double amount), bool) {
    return false;
}

define_method(
    approve, (const Employee& r, const Public& e, double amount), bool) {
    return true;
}

define_method(approve, (const Manager& r, const Taxi& e, double amount), bool) {
    return true;
}

define_method(
    approve, (const Founder& r, const Expense& e, double amount), bool) {
    return true;
}

int main() {
    yorel::yomm2::initialize();
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
