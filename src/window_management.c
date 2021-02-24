/*
 * Copyright 2019, 2021 Denis Salem
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
#include <string.h>
#include <time.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include "common.h"
#include "extensions.h"
#include "text_pointer.h"
#include "window_management.h"

unsigned int g_dstudio_mouse_state = 0;
long g_x11_input_mask = 0;

static int s_focus_type;

UIInteractiveList * g_active_interactive_list = 0;

static void (*cursor_position_callback)(int xpos, int ypos) = 0;
void (*mouse_button_callback)(int xpos, int ypos, int button, int action) = 0;

void (*close_sub_menu_callback)() = NULL;

#ifdef DSTUDIO_RELY_ON_X11

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static int ctx_error_occurred = 0;
static int window_alive = 1;
static int refresh_all = 1;
static Display              *display = NULL;
static Window               window;
static XVisualInfo          * visual_info;
static Colormap             color_map;
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
      GLX_SWAP_METHOD_OML , 0x8062, // Might prevent glitching with default desired setting.
      None
};

static int glx_major, glx_minor;
static unsigned int keyboard_chars_map_mode = 0;
static int pointer_x = 0;
static int pointer_y = 0;
int (*default_error_handler)(Display*, XErrorEvent*);

// Window states monitoring
static Atom s_wmState;
static Atom s_ret_type;
static int s_ret_format;
static unsigned long s_atom_iItem, s_atom_nItem, s_atom_bytesAfter;
static unsigned char *s_atom_properties = NULL;
static char * s_atom_name = 0;
static XWindowAttributes s_xwa;
static double s_wa_update_timestamp=0;
static int s_is_visible = 1;

static int ctx_error_handler( Display *dpy, XErrorEvent *ev ) {
    (void) dpy;
    (void) ev;
    ctx_error_occurred = 1;
    return 0;
}

static int runtime_error_handler( Display *dpy, XErrorEvent *ev ) {
    default_error_handler(dpy, ev);
    return 0;
}

static void creating_color_map(XVisualInfo * vi, Window * root_window, XSetWindowAttributes * swa) {
    color_map = XCreateColormap(display, *root_window, vi->visual, AllocNone);
    swa->colormap = color_map;
    swa->background_pixmap = None ;
    swa->border_pixel      = 0;
    swa->event_mask        = 0;
}

void configure_input(long mask) {
    pointer_x = -1;
    pointer_y = -1;
    g_x11_input_mask = DSTUDIO_X11_INPUT_MASKS ^ mask;
    XSelectInput(display, window, g_x11_input_mask);
}

void destroy_context() {
    glXDestroyContext(display, opengl_context);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int do_no_exit_loop() {
    return window_alive;
}

void get_pointer_coordinates(int * x, int * y) {
    *x = pointer_x;
    *y = pointer_y;
}

static void get_visual_info(GLXFBConfig * best_frame_buffer_config) {
    int fbcount;
    int best_frame_buffer_config_index = -1;
    
    GLXFBConfig * frame_buffer_config = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (frame_buffer_config == NULL) { 
        const char *glx_exts = glXQueryExtensionsString(display, DefaultScreen( display ));
        if (is_extension_supported( glx_exts, "GLX_OML_swap_method" )) {
            visual_attribs[23] = 0x8063; // Set GLX_SWAP_METHOD_OML to GLX_SWAP_UNDEFINED_OML
        }
        else {
            visual_attribs[20] = 0;
        }
        frame_buffer_config = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
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

    XFree( frame_buffer_config );
    
    // Get a visual
    visual_info = glXGetVisualFromFBConfig( display, *best_frame_buffer_config );
    #ifdef DSTUDIO_DEBUG
        printf( "Chosen visual ID = 0x%lux\n", visual_info->visualid );
    #endif
}

void init_context(const char * window_name, int width, int height) {
    DSTUDIO_EXIT_IF_NULL(XInitThreads());
    default_error_handler = XSetErrorHandler(runtime_error_handler);
    int (*old_handler)(Display*, XErrorEvent*) = XSetErrorHandler(ctx_error_handler);
    Window root_window;
    XSetWindowAttributes swa;
    XSizeHints * size_hints;

    display = XOpenDisplay(NULL);
    DSTUDIO_EXIT_IF_NULL(display)
    
    DSTUDIO_EXIT_IF_NULL(glXQueryVersion( display, &glx_major, &glx_minor ))
    DSTUDIO_EXIT_IF_FAILURE( glx_major == 1  &&  (glx_minor < 3))
    DSTUDIO_EXIT_IF_FAILURE( glx_major < 1 )

    GLXFBConfig best_frame_buffer_config;
    get_visual_info(&best_frame_buffer_config);
    DSTUDIO_EXIT_IF_NULL(visual_info)

    root_window = RootWindow(display, visual_info->screen);
    creating_color_map(visual_info, &root_window, &swa);

    // TODO: Investigate necessity to set CW* flags
    window = XCreateWindow(display, root_window, 0, 0, width, height, 0, visual_info->depth, InputOutput, visual_info->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);
    
    DSTUDIO_EXIT_IF_NULL(window)

    size_hints = XAllocSizeHints();
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width = width;
    size_hints->min_height = height;
    size_hints->max_width = width;
    size_hints->max_height = height;
        
    XSetWMSizeHints(display, window, size_hints, XA_WM_NORMAL_HINTS);
    Atom delWindow = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display , window, &delWindow, 1);
    configure_input(0);
    XkbSetDetectableAutoRepeat (display, 1, NULL);

    XFree(visual_info);
    XFree(size_hints);
    XMapWindow(display, window);
    XStoreName(display, window, window_name);
    
    // Begin OpenGL context creation
    
    const char *glx_exts = glXQueryExtensionsString(display, DefaultScreen( display ));
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
    
    #ifdef DSTUDIO_DEBUG
    printf("GLX_EXTS: %s\n", glx_exts);
    #endif
    
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
            GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        opengl_context = glXCreateContextAttribsARB(display, best_frame_buffer_config, 0, 1, context_attribs);
    }
    
    // Sync to ensure any errors generated are processed.
    
    XSync(display, 0);
    DSTUDIO_EXIT_IF_FAILURE( ctx_error_occurred && opengl_context == 0)
    XSetErrorHandler(old_handler);
    
    #ifdef DSTUDIO_DEBUG
        if (!glXIsDirect(display, opengl_context)) {
            printf("Indirect GLX rendering context obtained\n");
        }
        else {
            printf("Direct GLX rendering context obtained\n");
        }
    #endif
    glXMakeCurrent(display, window, opengl_context);
    s_wmState = XInternAtom(display, "_NET_WM_STATE", True);
}

int is_window_focus() {
    return s_focus_type == FocusIn? 1 : 0; 
}

// Based on this
// discussion https://www.linuxquestions.org/questions/programming-9/how-to-read-the-state-by-using-_net_wm_state-in-xlib-836879/
int is_window_visible() {
    double timestamp = get_timestamp();

    if ( (timestamp - s_wa_update_timestamp) * 1000000 > DSTUDIO_WINDOW_IDLING_TIMEOUT) {
        s_is_visible = 1;
        XGetWindowAttributes(display, window, &s_xwa);
        s_wa_update_timestamp = timestamp;
        if (s_xwa.map_state != IsViewable) {
            s_is_visible = 0;
        }
    
        // TODO: Is it redundant?
        XGetWindowProperty(
            display,
            window,
            s_wmState,
            0,
            (~0L),
            False,
            AnyPropertyType,
            &s_ret_type,
            &s_ret_format,
            &s_atom_nItem,
            &s_atom_bytesAfter,
            &s_atom_properties
        );
        
        for (s_atom_iItem = 0; s_atom_iItem < s_atom_nItem; ++s_atom_iItem) {
            s_atom_name = XGetAtomName(display,((Atom*)(s_atom_properties))[s_atom_iItem]);
            if (strcmp("_NET_WM_STATE_SHADED", s_atom_name) == 0 || strcmp("_NET_WM_STATE_HIDDEN", s_atom_name) == 0) {
                s_is_visible = 0;
                break;
            }
        }
        XFree(s_atom_properties);
    }
    return s_is_visible;
}


void listen_events() {
    void (*close_sub_menu_callback_swap)() = NULL;
    
    if (!is_window_visible()) {
        XFlush(display);
        return;
    }
    
    //~ // The good way to process event
    struct pollfd fds = {0};
    fds.fd = ConnectionNumber(display);
    fds.events = POLLIN;

    if (poll(&fds, 1, 20) > 0) {
        while(XPending(display)) {
            XNextEvent(display, &x_event);
            if(x_event.type == ClientMessage) {
                window_alive = 0;
                return;
            }
            else if (x_event.type == Expose) {
                refresh_all = 1;
            }
            else if(x_event.type == FocusIn || x_event.type == FocusOut ) {
                s_focus_type = x_event.type;
                g_x11_input_mask = DSTUDIO_X11_INPUT_MASKS ^ PointerMotionMask;
            }
            else if (x_event.type == ButtonPress) {
                if (x_event.xbutton.button == Button1) {
                    g_dstudio_mouse_state = 1;
                    mouse_button_callback(x_event.xbutton.x, x_event.xbutton.y, DSTUDIO_MOUSE_BUTTON_LEFT, DSTUDIO_MOUSE_BUTTON_PRESS);
                    return;
                }
                else if (x_event.xbutton.button == Button3) {
                    mouse_button_callback(x_event.xbutton.x, x_event.xbutton.y, DSTUDIO_MOUSE_BUTTON_RIGHT, DSTUDIO_MOUSE_BUTTON_PRESS);
                    return;
                }
                else if (g_active_interactive_list) {
                    switch(x_event.xbutton.button) {
                        case Button4:
                            scroll(g_active_interactive_list, -1);
                            break;
                            
                        case Button5:
                            scroll(g_active_interactive_list, 1);
                            break;
                        default:
                            break;
                    }
                }
                
                cursor_position_callback(x_event.xbutton.x, x_event.xbutton.y);
            }
            else if (x_event.type == ButtonRelease) {
                if (x_event.xbutton.button == Button1) {
                    g_dstudio_mouse_state = 0;
                    mouse_button_callback(x_event.xbutton.x, x_event.xbutton.y, DSTUDIO_MOUSE_BUTTON_LEFT, DSTUDIO_MOUSE_BUTTON_RELEASE);
                    return;
                }
                else if (x_event.xbutton.button == Button3) {
                    mouse_button_callback(x_event.xbutton.x, x_event.xbutton.y, DSTUDIO_MOUSE_BUTTON_RIGHT, DSTUDIO_MOUSE_BUTTON_RELEASE);
                    return;
                }
            }
            else if(x_event.type == MotionNotify) {
                pointer_x = x_event.xmotion.x;
                pointer_y = x_event.xmotion.y;
                cursor_position_callback(x_event.xbutton.x, x_event.xbutton.y);
            }
            else if(x_event.type == KeyPress) {
                if(x_event.xkey.keycode == DSTUDIO_KEY_CODE_ESC || x_event.xkey.keycode == DSTUDIO_KEY_CODE_ENTER) {
                    if (x_event.xkey.keycode == DSTUDIO_KEY_CODE_ESC && close_sub_menu_callback != NULL) {
                        close_sub_menu_callback_swap = close_sub_menu_callback;
                        close_sub_menu_callback = NULL;
                        close_sub_menu_callback_swap();
                        refresh_all = 1;
                    }
                    clear_text_pointer();
                }
                else if (x_event.xkey.keycode == DSTUDIO_KEY_CODE_SHIFT || x_event.xkey.keycode == DSTUDIO_KEY_CAPS_LOCK) {
                    keyboard_chars_map_mode ^= DSTUDIO_KEY_MAJ_BIT;
                }
                else if (x_event.xkey.keycode == DSTUDIO_KEY_BOTTOM_ARROW && g_active_interactive_list) {
                    clear_text_pointer();
                    g_active_interactive_list->scroll_bar->enabled = 0;
                    scroll(g_active_interactive_list, 1);
                }
                else if (x_event.xkey.keycode == DSTUDIO_KEY_TOP_ARROW && g_active_interactive_list) {
                    clear_text_pointer();
                    scroll(g_active_interactive_list, -1);
                }
                else if(g_text_pointer_context.active) {
                    update_text_box(
                        XLookupKeysym(&x_event.xkey, keyboard_chars_map_mode)
                    );
                }
            }
            else if(x_event.type == KeyRelease)  {
                if (x_event.xkey.keycode == DSTUDIO_KEY_CODE_SHIFT) {
                    keyboard_chars_map_mode ^= DSTUDIO_KEY_MAJ_BIT;
                }
                if ((x_event.xkey.keycode == DSTUDIO_KEY_BOTTOM_ARROW || x_event.xkey.keycode == DSTUDIO_KEY_TOP_ARROW ) && g_active_interactive_list && g_active_interactive_list && g_active_interactive_list->scroll_bar ) {
                    g_active_interactive_list->scroll_bar->enabled = 1;
                }
            }
        }
    }
}

int need_to_redraw_all() {
    if (refresh_all) {
        refresh_all = 0;
        if (g_menu_background_enabled) {
            g_menu_background_enabled->render_state = DSTUDIO_UI_ELEMENT_RENDER_REQUESTED;
        }
        return 1;
    }
    return 0;    
}

void set_close_sub_menu_callback(void (*callback)()) {
    close_sub_menu_callback = callback;
}


void set_cursor_position_callback(void (*callback)(int xpos, int ypos)) {
    cursor_position_callback = callback;
}

void set_mouse_button_callback(void (*callback)(int xpos, int ypos, int button, int action)) {
    mouse_button_callback = callback;
}

void swap_window_buffer() {
    glXSwapBuffers(display, window);
}

#endif
