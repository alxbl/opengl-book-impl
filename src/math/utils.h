#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static const double PI = 3.14159265358979323846;

typedef struct vertex_ {
    float pos[4];
    float color[4];
} vertex_t;

typedef struct mat4_ {
    float m[16];
} mat4_t;

extern const mat4_t IDENTITY4;

float cotangent(float angle);
float deg2rad(float deg);
float rad2deg(float rad);

mat4_t mat_mult(const mat4_t* m1, const mat4_t* m2);
void rot_x(mat4_t* m, float angle);
void rot_y(mat4_t* m, float angle);
void rot_z(mat4_t* m, float angle);
void scale(mat4_t* m, float x, float y, float z);
void translate(mat4_t* m, float x, float y, float z);


mat4_t proj(float fovy, float aspect_ratio, float near_plane, float far_plane);

void exit_on_glError(const char* msg);
GLuint load_shader(const char* filename, GLenum shader_type);

#endif // MATH_UTILS_H

