cmake_minimum_required(VERSION 3.10)
set(PROJ chapter4)
project(${PROJ})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Copy shader files to build output.
configure_file(simple.fragment.glsl simple.fragment.glsl COPYONLY)
configure_file(simple.vertex.glsl simple.vertex.glsl COPYONLY)

add_executable(${PROJ} "${PROJ}.c")
target_link_libraries(${PROJ} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} math)
