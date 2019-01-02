/* Goxel 3D voxels editor
 *
 * copyright (c) 2015 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "goxel.h"
#include <getopt.h>

#ifdef GLES2
#   define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

static inputs_t     *g_inputs = NULL;
static GLFWwindow   *g_window = NULL;
static float        g_scale = 1;

void on_scroll(GLFWwindow *win, double x, double y)
{
    g_inputs->mouse_wheel = y;
}

void on_char(GLFWwindow *win, unsigned int c)
{
    inputs_insert_char(g_inputs, c);
}

typedef struct
{
    char *input;
    char *export;
    char *script;
    int script_args_nb;
    const char *script_args[32];
    float scale;
} args_t;

#define OPT_SCRIPT 1

static void parse_options(int argc, char **argv, args_t *args)
{
    int c, option_index;
    static struct option long_options[] = {
        {"export", required_argument, 0, 'e'},
        {"scale", required_argument, 0, 's'},
        {"script", required_argument, 0, OPT_SCRIPT},
        {NULL, 0, NULL, 0}
    };
    while (true) {
        c = getopt_long(argc, argv, "e:s:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
        case 'e':
            args->export = optarg;
            break;
        case 's':
            args->scale = atof(optarg);
            break;
        case OPT_SCRIPT:
            args->script = optarg;
            break;
        case '?':
            exit(-1);
        }
    }
    if (optind < argc) {
        if (args->script) {
            while (optind < argc)
                args->script_args[args->script_args_nb++] = argv[optind++];
        } else {
            args->input = argv[optind];
        }
    }
}


static void loop_function(void) {

    int fb_size[2], win_size[2];
    int i;
    double xpos, ypos;
    float scale = g_scale;

    if (    !glfwGetWindowAttrib(g_window, GLFW_VISIBLE) ||
             glfwGetWindowAttrib(g_window, GLFW_ICONIFIED)) {
        glfwWaitEvents();
        goto end;
    }
    // The input struct gets all the values in framebuffer coordinates,
    // On retina display, this might not be the same as the window
    // size.
    glfwGetWindowSize(g_window, &win_size[0], &win_size[1]);
    glfwGetFramebufferSize(g_window, &fb_size[0], &fb_size[1]);
    g_inputs->window_size[0] = win_size[0] / scale;
    g_inputs->window_size[1] = win_size[1] / scale;
    g_inputs->scale = fb_size[0] / win_size[0] * scale;

    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    for (i = 0; i <= GLFW_KEY_LAST; i++) {
        g_inputs->keys[i] = glfwGetKey(g_window, i) == GLFW_PRESS;
    }
    glfwGetCursorPos(g_window, &xpos, &ypos);
    vec2_set(g_inputs->touches[0].pos, xpos / scale, ypos / scale);
    g_inputs->touches[0].down[0] =
        glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    g_inputs->touches[0].down[1] =
        glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    g_inputs->touches[0].down[2] =
        glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    goxel_iter(g_inputs);
    goxel_render();

    memset(g_inputs, 0, sizeof(*g_inputs));
    glfwSwapBuffers(g_window);
end:
    glfwPollEvents();
}

#ifndef __EMSCRIPTEN__
static void start_main_loop(void (*func)(void))
{
    while (!glfwWindowShouldClose(g_window)) {
        func();
        if (goxel.quit) break;
    }
    glfwTerminate();
}
#else
static void start_main_loop(void (*func)(void))
{
    emscripten_set_main_loop(func, 0, 1);
}
#endif

#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 2

static void load_icon(GLFWimage *image, const char *path)
{
    uint8_t *img;
    int w, h, bpp = 0;
    img = img_read(path, &w, &h, &bpp);
    assert(img);
    assert(bpp == 4);
    image->width = w;
    image->height = h;
    image->pixels = img;
}

static void set_window_icon(GLFWwindow *window)
{
    GLFWimage icons[7];
    int i;
    load_icon(&icons[0], "asset://data/icons/icon16.png");
    load_icon(&icons[1], "asset://data/icons/icon24.png");
    load_icon(&icons[2], "asset://data/icons/icon32.png");
    load_icon(&icons[3], "asset://data/icons/icon48.png");
    load_icon(&icons[4], "asset://data/icons/icon64.png");
    load_icon(&icons[5], "asset://data/icons/icon128.png");
    load_icon(&icons[6], "asset://data/icons/icon256.png");
    glfwSetWindowIcon(window, 7, icons);
    for (i = 0; i < 7; i++) free(icons[i].pixels);
}

#else
static void set_window_icon(GLFWwindow *window) {}
#endif

static void set_window_title(void *user, const char *title)
{
    glfwSetWindowTitle(g_window, title);
}

int main(int argc, char **argv)
{
    args_t args = {.scale = 1};
    GLFWwindow *window;
    GLFWmonitor *monitor;
    const GLFWvidmode *mode;
    int ret = 0;
    inputs_t inputs = {};
    g_inputs = &inputs;

    // Setup sys callbacks.
    sys_callbacks.set_window_title = set_window_title;
    parse_options(argc, argv, &args);

    g_scale = args.scale;

    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    window = glfwCreateWindow(mode->width, mode->height, "Goxel", NULL, NULL);
    assert(window);
    g_window = window;
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, on_scroll);
    glfwSetCharCallback(window, on_char);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, false);
    set_window_icon(window);

#ifdef WIN32
    glewInit();
#endif
    goxel_init();
    // Run the unit tests in debug.
    if (DEBUG) {
        tests_run();
        goxel_reset();
    }

    if (args.input)
        action_exec2("import", "p", args.input);

    if (args.script) {
        script_run(args.script, args.script_args_nb, args.script_args);
        goto end;
    }

    if (args.export) {
        if (!args.input) {
            LOG_E("trying to export an empty image");
            ret = -1;
        } else {
            ret = action_exec2("export", "p", args.export);
        }
        goto end;
    }
    start_main_loop(loop_function);
end:
    glfwTerminate();
    goxel_release();
    return ret;
}
