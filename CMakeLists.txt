cmake_minimum_required(VERSION 3.10)
project(openGLBook)

include_directories(src)

# Allow chapters to see math library.
link_directories(src/math)
add_subdirectory(src/math)

add_subdirectory(src/chapter1)
add_subdirectory(src/chapter2)
add_subdirectory(src/chapter3)
add_subdirectory(src/chapter4)

