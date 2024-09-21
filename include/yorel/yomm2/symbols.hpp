// Copyright (c) 2018-2024 Jean-Louis Leroy

#ifndef YOMM2_GENSYM

#include <boost/preprocessor/cat.hpp>

#define YOMM2_GENSYM BOOST_PP_CAT(yomm2_gensym_, __COUNTER__)

#define YOMM2_REGISTER(...) static __VA_ARGS__ YOMM2_GENSYM

#endif
