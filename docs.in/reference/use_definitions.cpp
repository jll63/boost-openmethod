#ifdef YOMM2_MD

experimental: use_definitions
headers: yorel/yomm2/templates.hpp

```c++
template<template<typename...> typename Definition, typename LoL>
struct use_definitions;
```

`use_definitions` takes a templatized definition container `Definition`, and a
[type list](mp_list.md) of type lists, each consisting of a method class, followed
by any number of mp_list. It instantiates `Definition` with each type list, and,
if the resulting class does not derive from ->`not_defined`, it adds the
definition container to the method, using ->`method::override`.

Typically, `LoL` is generated by the ->product meta-function.

## Example

#endif

#define BOOST_TEST_MODULE runtime
#include <boost/test/included/unit_test.hpp>

#ifdef YOMM2_CODE

struct abstract_matrix { virtual ~abstract_matrix() {} };
struct ordinary_matrix : abstract_matrix {};
struct square_matrix : abstract_matrix {};
struct sparse_matrix : abstract_matrix {};

#include <yorel/yomm2/core.hpp>
#include <yorel/yomm2/compiler.hpp>
#include <yorel/yomm2/symbols.hpp>
#include <yorel/yomm2/templates.hpp>

using namespace yorel::yomm2; // for brevity
using boost::mp11::mp_list;

use_classes<
    abstract_matrix, ordinary_matrix, square_matrix, sparse_matrix
> YOMM2_GENSYM;

struct YOMM2_SYMBOL(same_type);
using same_type = method<
    YOMM2_SYMBOL(same_type),
    bool(virtual_<const abstract_matrix&>, virtual_<const abstract_matrix&>)
>;

// 1 - see notes below
bool same_type_catch_all(const abstract_matrix&, const abstract_matrix&) {
    return false;
}
YOMM2_STATIC(same_type::override_fn<same_type_catch_all>);

// 2
template<class Method, typename...>
struct definition : not_defined {};

// 3
template<typename Matrix>
struct definition<same_type, Matrix, Matrix> {
    static bool fn(const Matrix&, const Matrix&) {
        return true;
    }
};

use_definitions<
    definition,
    product< // 4
        mp_list<same_type>,
        mp_list<ordinary_matrix, square_matrix, sparse_matrix>,
        mp_list<ordinary_matrix, square_matrix, sparse_matrix>
    >
> YOMM2_GENSYM; // 5

BOOST_AUTO_TEST_CASE(ref_use_definitions) {
    initialize();

    const abstract_matrix& ordinary = ordinary_matrix();
    const abstract_matrix& square = square_matrix();
    const abstract_matrix& sparse = sparse_matrix();

    BOOST_TEST(same_type::fn(ordinary, ordinary) == true);
    BOOST_TEST(same_type::fn(ordinary, square) == false);
    BOOST_TEST(same_type::fn(sparse, square) == false);
}

#endif

#ifdef YOMM2_MD

1. By default, return `false`. This definition will be called if it is not
   overriden.
2. Declare the generic definition container. By default, it inherits from
   `not_defined`, and `use_definitions` will discard it, unless it is
   specialized.
3. If the method is `same_type`, and the second and third template arguments
   are the same matrix type, provide a definition that returns `true`.
4. Form the Cartesian product of the (binary) methods, the concrete classes
   (corresponding to the first binary method argument), and the concrete
   classes (corresponding to the second argument). It consists of 1x3x3
   tuples, each made of a method class from the first list, a matrix class
   from the second list, and a matrix class from the third list.
5. `use_classes` instantiates `definition` for each tuple in the cartesian
   product, and discards the classes that derived from `not_defined`. It adds
   the remaining definitions to the method (passed in the first template
   argument) using `method::override`.

#endif
