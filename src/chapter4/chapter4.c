#include "math/utils.h"

#define WINDOW_TITLE_PREFIX "Chapter 4"

int g_width = 500,
    g_height = 500,
    g_hwnd = 0;

unsigned int frames = 0;

GLuint proj_uloc, view_uloc, model_uloc, buffers[3] = { 0 }, shaders[3] = {0};

mat4_t proj_mat, view_mat, model_mat;

float cube_rot = 0;
clock_t last_time = 0;

void init(int, char*[]);
void init_wnd(int, char*[]);
void resize(int, int);
void render(void);

// timer handler
void on_timer(int);
void on_idle(void);

// Cube functions
void create_cube(void);
void delete_cube(void);
void draw_cube(void);

void on_keyboard(unsigned char, int, int);

// shader stuff
void cleanup(void);

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

    glGetError();
    glClearColor(0., 0., 0., 0.);

    // Setup culling
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    exit_on_glError("ERROR: Unable to configure culling.");

    model_mat = IDENTITY4;
    proj_mat = IDENTITY4;
    view_mat = IDENTITY4;

    translate(&view_mat, 0, 0, -2);

    create_cube();
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
    glutKeyboardFunc(on_keyboard);
}

void resize(int w, int h) {
    g_width = w;
    g_height = h;

    // x, y, w, h - bottom-left anchor point of the viewport.
    glViewport(0, 0, g_width, g_height);

    proj_mat = proj(60, (float)g_width / g_height, 1.0f, 100.0f);

    glUseProgram(shaders[0]);
    glUniformMatrix4fv(proj_uloc, 1, GL_FALSE, proj_mat.m);
    glUseProgram(0);
}

void render(void) {
    ++frames;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the frame

    draw_cube();

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
    delete_cube();
}

void on_keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'T':
        case 't':
            {
                break;
            }
        default: break;
    }
}

// Cube Functions
void create_cube(void) {
    const vertex_t vertices[8] = {
        { { -.5f, -.5f,  .5f, 1 }, { 0, 0, 1, 1 } },
        { { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },
        { {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
        { {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
        { { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
        { { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
        { {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
        { {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }
    };

    const GLuint indices[36] = {
        0,2,1,  0,3,2,
        4,3,0,  4,7,3,
        4,1,5,  4,0,1,
        3,6,2,  3,7,6,
        1,6,5,  1,2,6,
        7,5,6,  7,4,5
    };

    shaders[0] = glCreateProgram();
    exit_on_glError("ERROR: Could not create shader.");

    shaders[1] = load_shader("simple.fragment.glsl", GL_FRAGMENT_SHADER);
    shaders[2] = load_shader("simple.vertex.glsl", GL_VERTEX_SHADER);
        glAttachShader(shaders[0], shaders[1]);
        glAttachShader(shaders[0], shaders[2]);
    glLinkProgram(shaders[0]);
    exit_on_glError("ERROR: Could not link shader.");

    model_uloc = glGetUniformLocation(shaders[0], "ModelMatrix");
    view_uloc = glGetUniformLocation(shaders[0], "ViewMatrix");
    proj_uloc = glGetUniformLocation(shaders[0], "ProjectionMatrix");
    exit_on_glError("ERROR: Could not get shader uniform locations.");

    glGenBuffers(2, &buffers[1]);
    exit_on_glError("ERROR: Could not generate buffer objects.");

    glGenVertexArrays(1, &buffers[0]);
    exit_on_glError("ERROR: Could not generate VAO.");
    glBindVertexArray(buffers[0]);
    exit_on_glError("ERROR: Could not bind VAO.");


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    exit_on_glError("ERROR: Could not enable vertex attributes.");

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    exit_on_glError("ERROR: Could not bind buffer to VAO.");

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)0); // positions
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].pos)); // colors
    exit_on_glError("ERROR: Could not set VAO attributes.");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    exit_on_glError("ERROR: Could not bind index buffer to VAO.");

    glBindVertexArray(0);
}

void delete_cube(void) {
    glDetachShader(shaders[0], shaders[1]);
    glDetachShader(shaders[0], shaders[2]);

    glDeleteShader(shaders[1]);
    glDeleteShader(shaders[2]);
    glDeleteProgram(shaders[0]);

    exit_on_glError("ERROR: Could not destroy shaders.");

    glDeleteBuffers(2, &buffers[1]);
    glDeleteVertexArrays(1, &buffers[0]);
    exit_on_glError("ERROR: Could not destroy buffers.");
}

void draw_cube(void) {
    float angle;
    clock_t now = clock();

    if (last_time == 0) last_time == now;

    cube_rot += 360.0f * ((float)(now - last_time) / CLOCKS_PER_SEC);
    angle = deg2rad(cube_rot);
    last_time = now;

    model_mat = IDENTITY4;
    rot_y(&model_mat, angle);
    rot_x(&model_mat, angle);

    glUseProgram(shaders[0]);
    exit_on_glError("ERROR: Could not use shader program.");

    glUniformMatrix4fv(model_uloc, 1, GL_FALSE, model_mat.m);
    glUniformMatrix4fv(view_uloc, 1, GL_FALSE, view_mat.m);
    exit_on_glError("ERROR: Could not set shader uniforms.");

    glBindVertexArray(buffers[0]);
    exit_on_glError("ERROR: Could not bind VAO.");
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
    exit_on_glError("ERROR: Failed to draw elements.");

    // Unbind array
    glBindVertexArray(0);
    glUseProgram(0);
}
