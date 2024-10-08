# Copyright 2018, 2019 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(boost_openmethod VERSION 1.87.0 LANGUAGES CXX)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(boost_openmethod INTERFACE)
add_library(Boost::openmethod ALIAS boost_openmethod)

target_include_directories(boost_openmethod INTERFACE include)
target_compile_features(boost_openmethod INTERFACE cxx_alias_templates cxx_variadic_templates cxx_decltype)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

  # Fetch support files

  message(STATUS "Fetching BoostFetch.cmake")

  file(DOWNLOAD
    "https://raw.githubusercontent.com/boostorg/cmake/develop/include/BoostFetch.cmake"
    "${CMAKE_BINARY_DIR}/fetch_and_include/BoostFetch.cmake"
  )

  include("${CMAKE_BINARY_DIR}/fetch_and_include/BoostFetch.cmake")

  boost_fetch(boostorg/cmake TAG develop NO_ADD_SUBDIR)

  FetchContent_GetProperties(boostorg_cmake)

  list(APPEND CMAKE_MODULE_PATH ${boostorg_cmake_SOURCE_DIR}/include)

  # Testing (off by default)

  option(BUILD_TESTING "Build the tests." OFF)
  option(BUILD_EXAMPLES "Build the examples." OFF)
  include(CTest)

  if(BUILD_TESTING OR BUILD_EXAMPLES)

    set(BUILD_TESTING OFF) # hide cache variable

    boost_fetch(boostorg/assert TAG develop EXCLUDE_FROM_ALL)
    boost_fetch(boostorg/config TAG develop EXCLUDE_FROM_ALL)
    boost_fetch(boostorg/core TAG develop EXCLUDE_FROM_ALL)
    boost_fetch(boostorg/static_assert TAG develop EXCLUDE_FROM_ALL)
    boost_fetch(boostorg/throw_exception TAG develop EXCLUDE_FROM_ALL)

    unset(BUILD_TESTING)

  endif()

  # Do not use the default BoostInstall versioned layout on Windows when standalone
  set(BOOST_INSTALL_LAYOUT "system" CACHE STRING "Installation layout (versioned, tagged, or system)")

  include(BoostInstall)

  boost_install(TARGETS boost_openmethod HEADER_DIRECTORY include/)

endif()

if(BUILD_TESTING)
  add_subdirectory(test)
  set_property(TARGET tests PROPERTY EXCLUDE_FROM_ALL FALSE)
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
