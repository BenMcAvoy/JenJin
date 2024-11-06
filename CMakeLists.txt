cmake_minimum_required(VERSION 3.5)

# Build type
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Project declaration
project(Jenjin)

if (WIN32)
	message(STATUS "Manually enabling unicode support")
    add_compile_options(/utf-8)
endif()

add_subdirectory(megasource/libs/)
add_subdirectory(engine)
add_subdirectory(jenjin)
