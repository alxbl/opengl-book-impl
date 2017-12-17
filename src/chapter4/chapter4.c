#include "math/utils.h"

#define WINDOW_TITLE_PREFIX "Chapter 4 (GLFW)"

int g_width = 500,
    g_height = 500;
GLFWwindow* g_hwnd = NULL; // Render Window Handle
unsigned int frames = 0;
GLuint proj_uloc, view_uloc, model_uloc, buffers[3] = { 0 }, shaders[3] = {0};
mat4_t proj_mat, view_mat, model_mat;

float cube_rot = 0;
float last_time = 0;

void on_error(int error, const char* desc);
void init(int, char*[]);
void init_wnd(int, char*[]);
void resize(GLFWwindow*, int, int);
void render(void);

void update_fps(float elapsed);
void on_idle(void);

// Cube functions
void create_cube(void);
void delete_cube(void);
void draw_cube(void);
void on_keyboard(GLFWwindow*, int, int, int, int);
void cleanup(void);

int main(int argc, char* argv[]) {
    init(argc, argv);

    // Rendering loop.
    float now, prev, delta;
    now = prev = glfwGetTime();
    update_fps(0);
    while (!glfwWindowShouldClose(g_hwnd)) {
        render();
        now = glfwGetTime();
        delta = now - prev;
        ++frames;
        if ( delta > 1) {
            update_fps(delta);
            prev = now;
        }

        glfwPollEvents();
    }
    printf("Exiting...\n");

    glfwDestroyWindow(g_hwnd);
    glfwTerminate(); // GLFW must be terminated before the application exits
    exit(EXIT_SUCCESS);
}

void init(int argc, char* argv[]) {
    init_wnd(argc, argv);

    GLenum glew_res;
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
    glDepthFunc(GL_LESS);
    exit_on_glError("ERROR: Could not set depth testing.");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    exit_on_glError("ERROR: Unable to configure culling.");

    model_mat = IDENTITY4;
    proj_mat = IDENTITY4;
    view_mat = IDENTITY4;

    translate(&view_mat, 0, 0, -2);

    create_cube();

    // Initialize the viewport.
    glfwSetFramebufferSizeCallback(g_hwnd, resize);
    glfwGetFramebufferSize(g_hwnd, &g_width, &g_height);
    resize(g_hwnd, g_width, g_height); // Initial resize call.

    // Configure Event callbacks
    glfwSetKeyCallback(g_hwnd, on_keyboard);
}

void init_wnd(int argc, char* argv[]) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    // Add a callback to be notified about GLFW errors.
    glfwSetErrorCallback(on_error); 

    // Create the rendering viewport.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Require OpenGL > 4
    g_hwnd = glfwCreateWindow(g_width, g_height, WINDOW_TITLE_PREFIX, NULL, NULL);

    if (!g_hwnd) {
        fprintf(stderr, "ERROR: Could not create rendering window!\n");
        exit(1);
    }

    glfwMakeContextCurrent(g_hwnd);
}

void resize(GLFWwindow* wnd, int w, int h) {
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the frame
    draw_cube();
    glfwSwapBuffers(g_hwnd);

}

void update_fps(float elapsed) {
    char* title = (char*) malloc(512 + strlen(WINDOW_TITLE_PREFIX));
    sprintf(title, "%s (%.2f fps @ %d x %d)",
            WINDOW_TITLE_PREFIX,
            elapsed > 0. ? (float)frames / elapsed : 0.,
            g_width,
            g_height
    );

    glfwSetWindowTitle(g_hwnd, title);
    free(title);
    frames = 0; // reset frame counter.
}

void cleanup(void) {
    delete_cube();
}

void on_keyboard(GLFWwindow* wnd, int key, int scan, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(g_hwnd, GLFW_TRUE);
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
    float now = glfwGetTime();

    if (last_time == 0.) last_time = now;

    cube_rot += 45.0f * ((float)(now - last_time));
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

void on_error(int error, const char* desc) {
    fprintf(stderr, "ERROR (%d): %s.\n", error, desc);
    exit(EXIT_FAILURE);
}
