/*
 * Copyright 2019 Denis Salem
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
