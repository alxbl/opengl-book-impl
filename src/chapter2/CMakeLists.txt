cmake_minimum_required(VERSION 3.10)
project(chapter2)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
add_executable(chapter2 chapter2.c)
target_link_libraries(chapter2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
