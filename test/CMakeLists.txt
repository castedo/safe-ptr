cmake_minimum_required(VERSION 2.8.1)
project(safe_ptr)


include_directories(..)

add_executable(test_safe_ptr test_safe_ptr.cpp test_make_safe.cpp)
target_link_libraries(test_safe_ptr boost_unit_test_framework)


# gcc settings
add_definitions(-std=c++0x -Wall -Wno-deprecated)

# gcc settings for debug build
add_definitions(-g -O0 -fno-inline -fno-eliminate-unused-debug-types)

# gcc settings for optimized non-debug build
#add_definitions(-O3)

