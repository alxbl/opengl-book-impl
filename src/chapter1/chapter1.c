#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define WINDOW_TITLE_PREFIX "Chapter 1"

int g_width = 500,
    g_height = 500,
    g_hwnd = 0;

unsigned int frames = 0;


void init(int, char*[]);
void init_wnd(int, char*[]);
void resize(int, int);
void render(void);

// timer handler
void on_timer(int);
void on_idle(void);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
