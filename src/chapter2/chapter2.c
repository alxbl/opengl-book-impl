#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define WINDOW_TITLE_PREFIX "Chapter 2"

int g_width = 500,
    g_height = 500,
    g_hwnd = 0;

unsigned int frames = 0;

GLuint vertex_shader_id, fragment_shader_id, prog_id, vao_id, vbo_id, color_buf_id;

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
    printf("OpenGL Book: Chapter 1\n");

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

    glDrawArrays(GL_TRIANGLES, 0 , 3);

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
    GLfloat vertices[] = { // Some hardcoded vertices from the book.
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.0f,  0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f, // red
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 0.0f, 1.0f, 1.0f, // blue
    };

    GLenum error = glGetError();

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    // Vertices
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Colors
    glGenBuffers(1, &color_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buf_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

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
    glDeleteBuffers(1, &color_buf_id);
    glDeleteBuffers(1, &vbo_id);

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
