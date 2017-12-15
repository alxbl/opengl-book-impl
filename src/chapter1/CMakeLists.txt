cmake_minimum_required(VERSION 3.10)
project(chapter1)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
add_executable(chapter1 chapter1.c)
target_link_libraries(chapter1 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
