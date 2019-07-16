#include<GL/gl.h>
#include<GL/glx.h>

#ifndef WINDOW_MANAGEMENT_INCLUDED
#define WINDOW_MANAGEMENT_INCLUDED

void destroy_context();
void init_context(const char * window_name, int width, int height);
void swap_window_buffer();

#endif
