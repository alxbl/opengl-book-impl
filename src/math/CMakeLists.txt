cmake_minimum_required(VERSION 3.10)
set(PROJ math)
project(${PROJ})

set(SRCS utils.c)
set(HDRS utils.h)

find_package(GLEW REQUIRED)

add_library(${PROJ} STATIC ${SRCS} ${HDRS})
target_link_libraries(${PROJ} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} m)
