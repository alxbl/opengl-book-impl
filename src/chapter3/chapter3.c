#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define WINDOW_TITLE_PREFIX "Chapter 3"

typedef struct {
    float v[4];
    float c[4];
} Vertex;

int g_width = 500,
    g_height = 500,
    g_hwnd = 0;

unsigned int frames = 0;

GLuint vertex_shader_id, fragment_shader_id, prog_id, vao_id, vbo_id, ibo_id;

// A very simple vertex shader
const GLchar* v_shader = {
    "#version 400\n"
        "layout(location=0) in vec4 in_Position;\n"
        "layout(location=1) in vec4 in_Color;\n"
        "out vec4 ex_Color;\n"

        "void main(void)\n"
        "{\n"
        "  gl_Position = in_Position;\n"
        "  ex_Color = in_Color;\n"
        "}\n"
};

// And a simple fragment shader to go with it.
const GLchar* f_shader = {
    "#version 400 \n"
        "in vec4 ex_Color;\n"
        "out vec4 out_Color;\n"

        "void main(void)\n"
        "{\n"
        "  out_Color = ex_Color;\n"
        "}\n"
};
void init(int, char*[]);
void init_wnd(int, char*[]);
void resize(int, int);
void render(void);

// timer handler
void on_timer(int);
void on_idle(void);

// shader stuff
void cleanup(void);
void create_vbo(void);
void delete_vbo(void);
void create_shaders(void);
void delete_shaders(void);

int main(int argc, char* argv[]) {
    init(argc, argv);
    glutMainLoop();
    printf("Exiting...\n");
    exit(EXIT_SUCCESS);
}

void init(int argc, char* argv[]) {
    GLenum glew_res;

    init_wnd(argc, argv);

    glew_res = glewInit();

    if (glew_res != GLEW_OK) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(glew_res));
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Open GL Version: %s\n", glGetString(GL_VERSION));

    create_shaders();
    create_vbo();
    glClearColor(0., 0., 0., 0.);
}

void init_wnd(int argc, char* argv[]) {
    glutInit(&argc, argv); // Initialize the OpenGL engine

    glutInitContextVersion(4,0);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(
            GLUT_ACTION_ON_WINDOW_CLOSE,
            GLUT_ACTION_GLUTMAINLOOP_RETURNS
            );

    glutInitWindowSize(g_width, g_height);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    g_hwnd = glutCreateWindow(WINDOW_TITLE_PREFIX);

    if (g_hwnd < 1) {
        fprintf(stderr, "ERROR: Could not create rendering window!\n");
        exit(1);
    }

    glutReshapeFunc(resize);
    glutDisplayFunc(render);
    glutIdleFunc(on_idle);
    glutTimerFunc(0, on_timer, 0); // delay, func, arg
    glutCloseFunc(cleanup);
}

void resize(int w, int h) {
    g_width = w;
    g_height = h;

    // x, y, w, h - bottom-left anchor point of the viewport.
    glViewport(0, 0, g_width, g_height);
    // printf("Resized: %dx%d\n", w, h);
}

void render(void) {
    ++frames;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the frame

    // glDrawArrays(GL_TRIANGLES, 0 , 3); // Drawing only triangles...
    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*) 0);

    // Done painting, swap the buffer to the screen.
    glutSwapBuffers();
    glutPostRedisplay();
}

void on_idle(void) {
    glutPostRedisplay();
}

void on_timer(int val) {
    if (val != 0) {
        char* title = (char*) malloc(512 + strlen(WINDOW_TITLE_PREFIX));
        sprintf(title, "%s (%d fps @ %d x %d)",
                WINDOW_TITLE_PREFIX,
                frames * 4, // 250ms
                g_width,
                g_height
               );

        glutSetWindowTitle(title);
        free(title);
    }

    frames = 0; // reset frame counter.
    glutTimerFunc(250, on_timer, 1); // Re-arm the timer.
}

void cleanup(void) {
    delete_shaders();
    delete_vbo();
}

void create_vbo(void) {
    // Define all the vertices a single time.
    Vertex vertices[] =
    {
        //  Vertex Coords                Color
        { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        // Top
        { { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Bottom
        { { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Left
        { { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Right
        { { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
    };

    // Define each triangle using the index of the vertices
    GLubyte indices[] = {
        // Top
        0, 1, 3,
        0, 3, 2,
        3, 1, 4,
        3, 4, 2,

        0, 5, 7,
        0, 7, 6,
        7, 5, 8,
        7, 8, 6,

        0, 9, 11,
        0, 11, 10,
        11, 9, 12,
        11, 12, 10,

        0, 13, 15,
        0, 15, 14,
        15, 13, 16,
        15, 16, 14

    };

    GLenum error = glGetError();

    const size_t buffer_size = sizeof(vertices);
    const size_t vertex_size = sizeof(vertices[0]);
    const size_t color_offset = sizeof(vertices[0].v);

    // Generate the Vertex Array Object and bind it.
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    // Generate the vertex buffer.
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);

    // Define the data structure
    // glVertexAttribPointer(layout, numvert, type, normalize, stride, ptr)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertex_size, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)color_offset);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Create the index buffer that will contain the triangle declarations.
    glGenBuffers(1, &ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    error = glGetError();

    if (error != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: Could not create VBO: %s\n", gluErrorString(error));
        exit(-1);
    }

}

void delete_vbo(void) {
    GLenum error = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo_id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind the index buffer
    glDeleteBuffers(1, &ibo_id);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao_id);

    error = glGetError();

    if (error != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: Could not destroy VBO: %s\n", gluErrorString(error));
        exit(-1);
    }
}

void create_shaders(void) {
    GLenum error = glGetError();

    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &v_shader, NULL);
    glCompileShader(vertex_shader_id);

    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &f_shader, NULL);
    glCompileShader(fragment_shader_id);

    prog_id = glCreateProgram();
    glAttachShader(prog_id, vertex_shader_id);
    glAttachShader(prog_id, fragment_shader_id);
    glLinkProgram(prog_id);
    glUseProgram(prog_id);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: Could not create shaders: %s\n", gluErrorString(error));
        exit(-1);
    }
}

void delete_shaders(void) {
    GLenum error = glGetError();

    glUseProgram(0);
    glDetachShader(prog_id, vertex_shader_id);
    glDetachShader(prog_id, fragment_shader_id);

    glDeleteShader(fragment_shader_id);
    glDeleteShader(vertex_shader_id);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: Could not create shaders: %s\n", gluErrorString(error));
        exit(-1);
    }
}
