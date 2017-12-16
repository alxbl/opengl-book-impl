#include "utils.h"

const mat4_t IDENTITY4 = {
    {
        //          To help visualize
        1, 0, 0, 0, //  0,  1,  2,  3
        0, 1, 0, 0, //  4,  5,  6,  7
        0, 0, 1, 0, //  8,  9, 10, 11
        0, 0, 0, 1  // 12, 13, 14, 15
    }
};

// Simplest, most-straight forward implementations provided by the book.
// Also: No arithmetic checks! This code is NOT meant for a real 3D application.

float cotangent(float angle) { return (float)(1.0f / tan(angle)); }
float deg2rad(float deg) { return deg * (float)(PI / 180); }
float rad2deg(float rad) { return rad * (float)(180 / PI); }

mat4_t mat_mult(const mat4_t* m1, const mat4_t* m2) {
    mat4_t out = IDENTITY4;
    unsigned int row, column, row_offset;

    for (row = 0, row_offset = row * 4; row < 4; ++row, row_offset = row * 4)
        for (column = 0; column < 4; ++column)
            out.m[row_offset + column] =
                (m1->m[row_offset + 0] * m2->m[column + 0]) +
                (m1->m[row_offset + 1] * m2->m[column + 4]) +
                (m1->m[row_offset + 2] * m2->m[column + 8]) +
                (m1->m[row_offset + 3] * m2->m[column + 12]);
    return out;
}

void rot_x(mat4_t* m, float angle) {
    mat4_t rot = IDENTITY4;
    float sine   = (float)sin(angle);
    float cosine = (float)cos(angle);

    // See https://en.wikipedia.org/wiki/Rotation_matrices#Basic_rotations
    rot.m[5]  = cosine;
    rot.m[6]  = -sine;
    rot.m[9]  = sine;
    rot.m[10] = cosine;

    memcpy(m->m, mat_mult(m, &rot).m, sizeof(m->m));
}

void rot_y(mat4_t* m, float angle) {
    mat4_t rot = IDENTITY4;
    float sine   = (float)sin(angle);
    float cosine = (float)cos(angle);

    // See https://en.wikipedia.org/wiki/Rotation_matrices#Basic_rotations
    rot.m[0]  = cosine;
    rot.m[8]  = sine;
    rot.m[2]  = -sine;
    rot.m[10] = cosine;

    memcpy(m->m, mat_mult(m, &rot).m, sizeof(m->m));
}

void rot_z(mat4_t* m, float angle) {
    mat4_t rot = IDENTITY4;
    float sine   = (float)sin(angle);
    float cosine = (float)cos(angle);

    // See https://en.wikipedia.org/wiki/Rotation_matrices#Basic_rotations
    rot.m[0]  = cosine;
    rot.m[1]  = -sine;
    rot.m[4]  = sine;
    rot.m[5] = cosine;

    memcpy(m->m, mat_mult(m, &rot).m, sizeof(m->m));
}

void scale(mat4_t* m, float x, float y, float z) {
    mat4_t scale = IDENTITY4;
    // The 3x3 identity acts as a scaling factor.
    scale.m[0] = x;
    scale.m[5] = y;
    scale.m[10] = z;
    memcpy(m->m, mat_mult(m, &scale).m, sizeof(m->m));
}

void translate(mat4_t* m, float x, float y, float z) {
    mat4_t tl = IDENTITY4;
    // The bottom-most row (0,4) .. (3,4) in the matrix acts as a translation offset.
    tl.m[12] = x;
    tl.m[13] = y;
    tl.m[14] = z;

    memcpy(m->m, mat_mult(m, &tl).m, sizeof(m->m));
}

mat4_t proj(float fovy, float aspect_ratio, float near_plane, float far_plane) {
    mat4_t out = { {0} };
    const float y_scale = cotangent(deg2rad(fovy/2)),
                x_scale = y_scale / aspect_ratio,
                frustum_len = far_plane - near_plane;

    out.m[0]  = x_scale;
    out.m[5]  = y_scale;
    out.m[10] = -((far_plane + near_plane) / frustum_len);
    out.m[11] = -1;
    out.m[14] = -((2 * near_plane * far_plane) / frustum_len);

    return out;
}


void exit_on_glError(const char* msg) {
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "%s: %s\n", msg, gluErrorString(error));
        exit(EXIT_FAILURE);
    }
}

GLuint load_shader(const char* filename, GLenum shader_type) {
    GLuint shader_id = 0;
    FILE* fd;
    long fsz = -1;
    char* glsl_src;

    if ((fd = fopen(filename, "rb")) != NULL
            && fseek(fd, 0, SEEK_END) == 0
            && (fsz = ftell(fd)) != -1) {

        rewind(fd);

        if ((glsl_src = (char*) malloc(fsz + 1)) != NULL) {
            // Read the shader in
            if (fsz == (long)fread(glsl_src, sizeof(char), fsz, fd)) {
                glsl_src[fsz] = '\0';

                const char* glsl = glsl_src; // Avoid compiler warning about non-const ptr.
                if ((shader_id = glCreateShader(shader_type)) != 0) {
                    glShaderSource(shader_id, 1, &glsl, NULL);
                    glCompileShader(shader_id);
                } else fprintf(stderr, "ERROR: Could not create shader.\n");
            } else fprintf(stderr, "ERROR: Could not read file.\n");
            free(glsl_src);
        } else fprintf(stderr, "ERROR: Could not allocate %d bytes.\n", fsz);
        fclose(fd);
    } else {
        if (fd != NULL) fclose(fd);
        fprintf(stderr, "ERROR: Could not open file.\n");
    }

    return shader_id;
}

