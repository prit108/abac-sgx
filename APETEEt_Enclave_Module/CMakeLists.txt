cmake_minimum_required(VERSION 3.13)
project(pybind_poltree)
include_directories(include)
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.6.2
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(pybind11)

pybind11_add_module(poltree app/app.cpp)
target_compile_features(poltree PUBLIC cxx_std_11)
set_target_properties(poltree PROPERTIES SUFFIX ".so")