project(synth)

cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
include(../conan/release/conanbuildinfo.cmake)
else()
include(../conan/debug/conanbuildinfo.cmake)
endif()
conan_basic_setup(TARGETS)

enable_testing()
add_subdirectory(src)