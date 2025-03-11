cmake_minimum_required(VERSION 3.4...3.18)
project(monero_cpp_binding)

set(CMAKE_CXX_STANDARD 11)
find_package(pybind11 REQUIRED)

# Adjust this path to point to the monero-cpp headers if necessary.
include_directories(${CMAKE_SOURCE_DIR}/monero-cpp/include)

pybind11_add_module(monero_cpp monero_cpp/bindings.cpp)

