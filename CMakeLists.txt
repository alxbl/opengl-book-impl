cmake_minimum_required(VERSION 3.10)
project(openGLBook)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(src)
include_directories(${GLFW_INCLUDE_DIRS})

# Allow chapters to see math library.
link_directories(src/math)
add_subdirectory(src/math)

add_subdirectory(src/chapter1)
add_subdirectory(src/chapter2)
add_subdirectory(src/chapter3)
add_subdirectory(src/chapter4)

