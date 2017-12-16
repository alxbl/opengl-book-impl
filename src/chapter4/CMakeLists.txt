cmake_minimum_required(VERSION 3.10)
set(PROJ chapter4)
project(${PROJ})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
add_executable(${PROJ} "${PROJ}.c")
target_link_libraries(${PROJ} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
