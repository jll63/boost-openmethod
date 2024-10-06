// Copyright (c) 2018-2024 Jean-Louis Leroy

#ifndef BOOST_OPENMETHOD_GENSYM

#include <boost/preprocessor/cat.hpp>

#define BOOST_OPENMETHOD_GENSYM BOOST_PP_CAT(openmethod_gensym_, __COUNTER__)

#define BOOST_OPENMETHOD_REGISTER(...)                                         \
    static __VA_ARGS__ BOOST_OPENMETHOD_GENSYM

#endif
