#include<GL/gl.h>
#include<GL/glx.h>

#ifndef WINDOW_MANAGEMENT_INCLUDED
#define WINDOW_MANAGEMENT_INCLUDED

void destroy_context();
int do_no_exit_loop();
void init_context(const char * window_name, int width, int height);
void listen_events();
int need_to_redraw_all();
void swap_window_buffer();

#endif
