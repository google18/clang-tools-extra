cmake_minimum_required(VERSION 2.8.12)
project(my_project)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")

include_directories("/Library/Developer/CommandLineTools/usr/include/c++/v1")
add_subdirectory(abseil-cpp)

add_executable(test-before TestBefore.cpp)
add_executable(test-after TestAfter.cpp)
target_link_libraries(test-before absl::memory)
target_link_libraries(test-after absl::memory)
