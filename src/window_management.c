#include <stdio.h>
#include <stdlib.h>
#include<X11/X.h>
#include<X11/Xatom.h>
#include<X11/Xlib.h>

#include "common.h"
#include "extensions.h"
#include "window_management.h"

#ifdef DSTUDIO_RELY_ON_X11

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092


typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static int ctx_error_occurred = 0;
static int window_alive = 1;
static int refresh_all = 1;
static Display              *display;
static Window               window;
static XVisualInfo          * visual_info;
static Colormap             color_map;
static XWindowAttributes    gwa;
static XEvent               x_event;
static GLXContext           opengl_context;
static int visual_attribs[] = {
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
      None
};

static int glx_major, glx_minor;

static int ctx_error_handler( Display *dpy, XErrorEvent *ev ) {
    ctx_error_occurred = 1;
    return 0;
}

static void creating_color_map(XVisualInfo * vi, Window * root_window, XSetWindowAttributes * swa) {
    color_map = XCreateColormap(display, *root_window, vi->visual, AllocNone);
    swa->colormap = color_map;
    swa->background_pixmap = None ;
    swa->border_pixel      = 0;
    swa->event_mask        = ExposureMask | KeyPressMask | StructureNotifyMask;
}

void destroy_context() {
    glXDestroyContext(display, opengl_context);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int do_no_exit_loop() {
    return window_alive;
}

static void get_visual_info(GLXFBConfig * best_frame_buffer_config) {
    int fbcount;
    int best_frame_buffer_config_index = -1;
    int worst_fbc = -1;
    int best_num_samp = -1;
    int worst_num_samp = 999;
    
    GLXFBConfig * frame_buffer_config = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    DSTUDIO_EXIT_IF_NULL(frame_buffer_config)

    for (int i=0; i<fbcount; ++i) {
        visual_info = glXGetVisualFromFBConfig( display, frame_buffer_config[i] );
        if (visual_info) {
            int samp_buf, samples;
            glXGetFBConfigAttrib( display, frame_buffer_config[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib( display, frame_buffer_config[i], GLX_SAMPLES       , &samples  );
            #ifdef DSTUDIO_DEBUG
                printf("Matching fbconfig %d, visual ID 0x%2lux: SAMPLE_BUFFERS = %d, SAMPLES = %d\n", i, visual_info->visualid, samp_buf, samples );
            #endif
            if ( best_frame_buffer_config_index < 0 || samp_buf && samples > best_num_samp ) {
                best_frame_buffer_config_index = i; 
                best_num_samp = samples;
            }
            if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp ) {
                worst_fbc = i;
                worst_num_samp = samples;
            }
        }
        XFree(visual_info);
    }
    
    *best_frame_buffer_config = frame_buffer_config[ best_frame_buffer_config_index ];
    XFree( frame_buffer_config );
    
    // Get a visual
    visual_info = glXGetVisualFromFBConfig( display, *best_frame_buffer_config );
    #ifdef DSTUDIO_DEBUG
        printf( "Chosen visual ID = 0x%lux\n", visual_info->visualid );
    #endif
}

void init_context(const char * window_name, int width, int height) {
    int (*old_handler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctx_error_handler);
    Window root_window;
    XSetWindowAttributes swa;
    XSizeHints * size_hints;

    display = XOpenDisplay(NULL);
    DSTUDIO_EXIT_IF_NULL(display)
    
    DSTUDIO_EXIT_IF_NULL(glXQueryVersion( display, &glx_major, &glx_minor ))
    DSTUDIO_EXIT_IF_FAILURE( glx_major == 1  &&  glx_minor < 3  )
    DSTUDIO_EXIT_IF_FAILURE( glx_major < 1 )

    GLXFBConfig best_frame_buffer_config;
    get_visual_info(&best_frame_buffer_config);
    DSTUDIO_EXIT_IF_NULL(visual_info)

    root_window = RootWindow(display, visual_info->screen);
    creating_color_map(visual_info, &root_window, &swa);

    window = XCreateWindow(display, root_window, 0, 0, width, height, 0, visual_info->depth, InputOutput, visual_info->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);
    
    size_hints = XAllocSizeHints();
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width = width;
    size_hints->min_height = height;
    size_hints->max_width = width;
    size_hints->max_height = height;
        
    XSetWMSizeHints(display, window, size_hints, XA_WM_NORMAL_HINTS);
    Atom delWindow = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display , window, &delWindow, 1);

    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);


    DSTUDIO_EXIT_IF_NULL(window)
    
    XFree(visual_info);
    XMapWindow(display, window);
    XStoreName(display, window, window_name);
    
    // Begin OpenGL context creation
    
    const char *glx_exts = glXQueryExtensionsString(display, DefaultScreen( display ));
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

    if ( !is_extension_supported( glx_exts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB ) {
        #ifdef DSTUDIO_DEBUG
            printf("glXCreateContextAttribsARB() not found. Using old-style GLX context.\n");
        #endif
        opengl_context = glXCreateNewContext( display, best_frame_buffer_config, GLX_RGBA_TYPE, 0, 1);
    }
    else {
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        opengl_context = glXCreateContextAttribsARB(display, best_frame_buffer_config, 0, 1, context_attribs);

        // Sync to ensure any errors generated are processed.
    }
    XSync(display, 0);
    DSTUDIO_EXIT_IF_FAILURE( ctx_error_occurred && opengl_context == 0)
    XSetErrorHandler(old_handler);
    
    #ifdef DSTUDIO_DEBUG
        if ( ! glXIsDirect ( display, opengl_context ) ) {
            printf( "Indirect GLX rendering context obtained\n" );
        }
        else {
            printf( "Direct GLX rendering context obtained\n" );
        }
    #endif
    glXMakeCurrent( display, window, opengl_context );
}

void listen_events() {
    XNextEvent(display, &x_event);
    if (x_event.type == ClientMessage) {
        window_alive = 0;
    }
    if (x_event.type == Expose) {
        refresh_all = x_event.type == Expose;
    }
    printf("Event\n");
}

int need_to_redraw_all() {
    return refresh_all;    
}

void swap_window_buffer() {
    glXSwapBuffers(display, window);
}



#endif

