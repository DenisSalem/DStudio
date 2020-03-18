/*
 * Copyright 2019, 2020 Denis Salem
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

#include "interactive_list.h"

#ifndef WINDOW_MANAGEMENT_INCLUDED
#define WINDOW_MANAGEMENT_INCLUDED

#define DSTUDIO_MOUSE_BUTTON_LEFT       1
#define DSTUDIO_MOUSE_BUTTON_RIGHT      2
#define DSTUDIO_MOUSE_BUTTON_PRESS      4
#define DSTUDIO_MOUSE_BUTTON_RELEASE    8
#define DSTUDIO_KEY_BOTTOM_ARROW        116
#define DSTUDIO_KEY_LEFT_ARROW          65361
#define DSTUDIO_KEY_RIGHT_ARROW         65363
#define DSTUDIO_KEY_TOP_ARROW           111
#define DSTUDIO_KEY_CODE_ENTER          36
#define DSTUDIO_KEY_CODE_ESC            9
#define DSTUDIO_KEY_CODE_ERASEBACK      65288
#define DSTUDIO_KEY_CODE_SHIFT          50
#define DSTUDIO_KEY_CAPS_LOCK           66           
#define DSTUDIO_KEY_MAJ_BIT             1

#define DSTUDIO_X11_INPUT_MASKS \
    (ExposureMask | \
    KeyPressMask | \
    KeyReleaseMask | \
    ButtonPressMask | \
    ButtonReleaseMask | \
    VisibilityChangeMask | \
    ButtonMotionMask)

void configure_input(long mask);
void destroy_context();
int do_no_exit_loop();
void get_pointer_coordinates(int * x, int * y);
void init_context(const char * window_name, int width, int height);
void listen_events();
int need_to_redraw_all();
void send_expose_event();
void set_close_sub_menu_callback(void (*callback)());
void set_cursor_position_callback(void (*callback)(int xpos, int ypos));
void set_mouse_button_callback(void (*callback)(int xpos, int ypos, int button, int action));
void swap_window_buffer();

extern unsigned int g_dstudio_mouse_state;
extern UIInteractiveList * g_active_interactive_list;
void (*close_sub_menu_callback)();
extern long g_x11_input_mask;
#endif
