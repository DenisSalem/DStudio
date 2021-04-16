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

#include <ctype.h>
#include <unistd.h>

#include "../fileutils.h"
#include "../flac.h"
#include "../instances.h"
#include "../open_file.h"
#include "../ui.h"
#include "../voices.h"
#include "../window_management.h"

#include "add.h"
#include "samples.h"
#include "ui.h"

static unsigned int load_sample(char * path, char * filename, FILE * file_fd) {
    SharedSample shared_sample = {};
    if (load_flac(file_fd, update_open_file_error, &shared_sample)) {
        shared_sample.identifier = dstudio_alloc(
            strlen(path)+strlen(filename)+1, // nullbyte
            DSTUDIO_FAILURE_IS_FATAL
        );
        strcat(shared_sample.identifier, path);
        strcat(shared_sample.identifier, filename);
        new_sample(filename, shared_sample);
        return 1;
    }
    return 0;
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
    (void) add_instance_file_descriptor();
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
