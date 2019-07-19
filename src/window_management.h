#include<GL/gl.h>
#include<GL/glx.h>

#ifndef WINDOW_MANAGEMENT_INCLUDED
#define WINDOW_MANAGEMENT_INCLUDED

#define DSTUDIO_MOUSE_BUTTON_LEFT       1
#define DSTUDIO_MOUSE_BUTTON_RIGHT      2
#define DSTUDIO_MOUSE_BUTTON_PRESS      4
#define DSTUDIO_MOUSE_BUTTON_RELEASE    8

void destroy_context();
int do_no_exit_loop();
void init_context(const char * window_name, int width, int height);
void listen_events();
int need_to_redraw_all();
void set_cursor_position_callback(void (*callback)(int xpos, int ypos));
void set_mouse_button_callback(void (*callback)(int xpos, int ypos, int button, int action));
void swap_window_buffer();

#endif
