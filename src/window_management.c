/*
 * Copyright 2019, 2023 Denis Salem
 *
 * This file is part of DStudio.
 *
 * DStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DStudio. If not, see <http://www.gnu.org/licenses/>.
*/

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "extensions.h"
#include "macros.h"
#include "window_management.h"

#ifdef DSTUDIO_RELY_ON_X11

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

#define DSTUDIO_X11_INPUT_MASKS \
    (ButtonMotionMask | \
    ButtonPressMask | \
    ButtonReleaseMask | \
    ExposureMask | \
    FocusChangeMask | \
    KeyPressMask | \
    KeyReleaseMask)
    
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static int           s_ctx_error_occurred = 0;
static Display       *s_display = NULL;
static GLXContext    s_opengl_context;
static int           s_pointer_x = 0;
static int           s_pointer_y = 0;
static Window        s_root_window;
static Window        s_window;
static uint_fast32_t s_window_alive = 1;

static int ctx_error_handler(Display *dpy, XErrorEvent *ev) {
    (void) dpy;
    (void) ev;
    s_ctx_error_occurred = 1;
    return 0;
}

static void creating_color_map(XVisualInfo * vi, XSetWindowAttributes * swa) {
    Colormap color_map = XCreateColormap(s_display, s_root_window, vi->visual, AllocNone);
    swa->colormap = color_map;
    swa->background_pixmap = None ;
    swa->border_pixel      = 0;
    swa->event_mask        = 0;
}

static void configure_input(long mask) {
    s_pointer_x = -1;
    s_pointer_y = -1;
    XSelectInput(s_display, s_window, DSTUDIO_X11_INPUT_MASKS ^ mask);
}

void destroy_window_context() {
    glXDestroyContext(s_display, s_opengl_context);
    XDestroyWindow(s_display, s_window);
    XCloseDisplay(s_display);
}

static XVisualInfo * get_visual_info(GLXFBConfig * best_frame_buffer_config) {
    int fbcount;
    int best_frame_buffer_config_index = -1;
    XVisualInfo * visual_info = NULL;
    int visual_attribs[] = {
          GLX_X_RENDERABLE    , True,
          GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
          GLX_RENDER_TYPE     , GLX_RGBA_BIT,
          GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
          GLX_RED_SIZE        , 8,
          GLX_GREEN_SIZE      , 8,
          GLX_BLUE_SIZE       , 8,
          GLX_ALPHA_SIZE      , 8,
          GLX_DEPTH_SIZE      , 24,
          GLX_STENCIL_SIZE    , 8,
          GLX_DOUBLEBUFFER    , True,
          GLX_SWAP_METHOD_OML , 0x8062, // Might prevent glitching with default desired setting.
          None
    };

    GLXFBConfig * frame_buffer_config = glXChooseFBConfig(
        s_display,
        DefaultScreen(s_display),
        visual_attribs,
        &fbcount
    );
    if (frame_buffer_config == NULL) { 
        const char *glx_exts = glXQueryExtensionsString(
            s_display,
            DefaultScreen(s_display)
        );
        if (is_extension_supported(glx_exts, "GLX_OML_swap_method")) {
            visual_attribs[23] = 0x8063; // Set GLX_SWAP_METHOD_OML to GLX_SWAP_UNDEFINED_OML
        }
        else {
            visual_attribs[20] = 0;
        }
        frame_buffer_config = glXChooseFBConfig(
            s_display,
            DefaultScreen(s_display),
            visual_attribs,
            &fbcount
        );
        DSTUDIO_EXIT_IF_NULL(frame_buffer_config)
        for (int i=0; i<fbcount; ++i) {
            visual_info = glXGetVisualFromFBConfig(
                s_display,
                frame_buffer_config[i]
            );
            if (visual_info) {
                int samp_buf, samples;
                glXGetFBConfigAttrib( s_display, frame_buffer_config[i], GLX_SAMPLE_BUFFERS, &samp_buf );
                glXGetFBConfigAttrib( s_display, frame_buffer_config[i], GLX_SAMPLES       , &samples  );
                #ifdef DSTUDIO_DEBUG
                    printf("Matching fbconfig %d, visual ID 0x%2lux: SAMPLE_BUFFERS = %d, SAMPLES = %d\n", i, visual_info->visualid, samp_buf, samples);
                #endif
                // Minimal settings are fine enough.
                if ( samp_buf >= 1 && samples >= 1) {
                    best_frame_buffer_config_index = i; 
                    XFree(visual_info);
                    break;
                }
            }
        }
        *best_frame_buffer_config = frame_buffer_config[ best_frame_buffer_config_index ];
    }
    else {
        *best_frame_buffer_config = frame_buffer_config[ 0 ];
    }

    XFree(frame_buffer_config);
    
    // Get a visual
    visual_info = glXGetVisualFromFBConfig(s_display, *best_frame_buffer_config);
    #ifdef DSTUDIO_DEBUG
        printf("Chosen visual ID = 0x%lux\n", visual_info->visualid);
    #endif
    return visual_info;
}

void init_window_context(const char * window_name, int width, int height) {
    DSTUDIO_EXIT_IF_NULL(XInitThreads());
    XSetErrorHandler(ctx_error_handler);
    
    XSetWindowAttributes swa;
    XSizeHints * size_hints;

    s_display = XOpenDisplay(NULL);
    DSTUDIO_EXIT_IF_NULL(s_display)
    
    int glx_major, glx_minor;
    
    DSTUDIO_EXIT_IF_NULL(glXQueryVersion(s_display, &glx_major, &glx_minor ))
    DSTUDIO_EXIT_IF_FAILURE( glx_major == 1  &&  (glx_minor < 3))
    DSTUDIO_EXIT_IF_FAILURE( glx_major < 1 )

    GLXFBConfig best_frame_buffer_config;
    XVisualInfo * visual_info = get_visual_info(&best_frame_buffer_config);
    DSTUDIO_EXIT_IF_NULL(visual_info)

    s_root_window = RootWindow(s_display, visual_info->screen);
    creating_color_map(visual_info, &swa);

    s_window = XCreateWindow(
        s_display,
        s_root_window,
        0,
        0,
        width,
        height,
        0,
        visual_info->depth,
        InputOutput,
        visual_info->visual, 
        CWBorderPixel|CWColormap|CWEventMask,
        &swa
    );
    
    DSTUDIO_EXIT_IF_NULL(s_window)

    size_hints = XAllocSizeHints();
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width = width;
    size_hints->min_height = height;
    size_hints->max_width = width;
    size_hints->max_height = height;
        
    XSetWMSizeHints(s_display, s_window, size_hints, XA_WM_NORMAL_HINTS);
    Atom delWindow = XInternAtom(s_display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(s_display , s_window, &delWindow, 1);
    configure_input(0);
    XkbSetDetectableAutoRepeat(s_display, 1, NULL);

    XFree(visual_info);
    XFree(size_hints);
    XMapWindow(s_display, s_window);
    XStoreName(s_display, s_window, (char *)window_name);
    
    // Begin OpenGL context creation
    
    const char *glx_exts = glXQueryExtensionsString(s_display, DefaultScreen(s_display));
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");
    
    #ifdef DSTUDIO_DEBUG
    printf("GLX_EXTS: %s\n", glx_exts);
    #endif
    
    if ( !is_extension_supported(glx_exts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB ) {
        #ifdef DSTUDIO_DEBUG
            printf("glXCreateContextAttribsARB() not found. Using old-style GLX context.\n");
        #endif
        s_opengl_context = glXCreateNewContext(s_display, best_frame_buffer_config, GLX_RGBA_TYPE, 0, 1);
    }
    else {
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            //GLX_CONTEXT_FLAGS_ARB       ,GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        s_opengl_context = glXCreateContextAttribsARB(
            s_display,
            best_frame_buffer_config,
            0,
            1,
            context_attribs
        );
    }
    
    // Sync to ensure any errors generated are processed.
    
    XSync(s_display, 0);
    DSTUDIO_EXIT_IF_FAILURE(s_ctx_error_occurred && s_opengl_context == 0)
    
    #ifdef DSTUDIO_DEBUG
        if (!glXIsDirect(s_display, s_opengl_context)) {
            printf("Indirect GLX rendering context obtained\n");
        }
        else {
            printf("Direct GLX rendering context obtained\n");
        }
    #endif
    glXMakeCurrent(s_display, s_window, s_opengl_context);
}

static void window_listen_events() {    
    struct pollfd fds = {0};
    XEve nt x_event;
    fds.fd = ConnectionNumber(s_display);
    fds.events = POLLIN;
    if (poll(&fds, 1, 20) > 0) {
        while(XPending(s_display)) {
            XNextEvent(s_display, &x_event);
            if(x_event.type == ClientMessage) {
                s_window_alive = 0;
                return;
            }
        }
    }
}

void window_render_loop() {
    double framerate_limiter = 0;
    double framerate_limiter_timestamp = 0;
    
        
    while (do_no_exit_loop()) {
            listen_events();
        
        usleep((uint_fast32_t) (framerate_limiter > 0 ? framerate_limiter : 0));
    }
};

#endif
