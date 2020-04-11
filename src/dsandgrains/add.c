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

#include "../instances.h"
#include "../open_file.h"
#include "../ui.h"
#include "../voices.h"
#include "../window_management.h"

#include "add.h"
#include "ui.h"

static void load_sample(char * filename, FILE * file_fd) {
    printf("%s\n", filename);
    (void) file_fd;
}

void add_sample(UIElements * ui_elements) {
    (void) ui_elements;
    close_add_sub_menu();
    
    open_file_menu(
        add_sub_menu_proxy,
        load_sample
    );
}

void add_instance(UIElements * ui_elements) {
    (void) ui_elements;
    add_instance_file_descriptor();
    close_add_sub_menu();
}

void add_voice(UIElements * ui_elements) {
    (void) ui_elements; 
    new_voice(DSTUDIO_USE_MUTEX);
    close_add_sub_menu();
}

void add_sub_menu(UIElements * ui_elements) {
    (void) ui_elements; 
    add_sub_menu_proxy();
}

void add_sub_menu_proxy() {
    configure_input(PointerMotionMask);
    set_prime_interface(0);
    set_ui_elements_visibility(&g_ui_elements_struct.menu_background, 1, 5);
    g_menu_background_enabled = &g_ui_elements_struct.menu_background;
    set_close_sub_menu_callback(close_add_sub_menu);
    g_request_render_all = 1;
}

void close_add_sub_menu() {
    configure_input(0);
    set_prime_interface(1);
    set_ui_elements_visibility(&g_ui_elements_struct.menu_background, 0, 5);
    g_menu_background_enabled = 0;
    g_active_interactive_list = 0;
    g_request_render_all = 1;
}
