cmake_minimum_required(VERSION 3.14)

project(SetLib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SetLib STATIC
  set.cpp
  set.h
  compact_pair.cpp
  compact_pair.h
)

target_compile_definitions(SetLib PRIVATE SETLIB_LIBRARY)
