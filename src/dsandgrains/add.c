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

#include "dsandgrains.h"

static uint_fast32_t load_sample(char * path, char * filename, FILE * file_fd) {
    SharedSample * shared_sample_p = 0;
    
    char * identifier = dstudio_alloc(
        strlen(path)+strlen(filename)+1, // nullbyte
        DSTUDIO_FAILURE_IS_FATAL
    );
    strcat(identifier, path);
    strcat(identifier, filename);
    
    shared_sample_p = lookup_shared_sample(identifier);
    dstudio_free(identifier);
    if (shared_sample_p->count++ || load_flac(file_fd, update_open_file_error, shared_sample_p)) {
        new_sample(filename, shared_sample_p);
        return 1;
    }

    return 0;
}

void add_sample(UIElements * ui_elements) {
    (void) ui_elements;
    close_add_sub_menu();
    
    open_file_menu(
        add_sub_menu_proxy,
        load_sample,
        filter_non_audio_file
    );
}

void add_instance(UIElements * ui_elements) {
    (void) ui_elements;
    (void) add_instance_file_descriptor();
    close_add_sub_menu();
}

void add_voice(UIElements * ui_elements) {
    (void) ui_elements; 
    new_voice();
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
    dstudio_clear_sub_menu_callback();
}

uint_fast32_t filter_non_audio_file(const char * path, const char * filename) {
    char * path_filename =  dstudio_alloc(sizeof(char) * strlen(path) + strlen(filename) + 2, DSTUDIO_FAILURE_IS_FATAL);
    strcat(path_filename, path);
    strcat(path_filename, "/");
    strcat(path_filename, filename);
    uint_fast32_t ret = 0;
    
    if (dstudio_is_directory(path_filename)) {
        ret =  1;
        goto end_filter;
    }
    
    char * string =  &path_filename[strlen(path_filename)-5]; // Look for .flac
    for ( ; *string; ++string) *string = tolower(*string);
    
    ret = strcmp(&path_filename[strlen(path_filename)-5], ".flac") == 0;
    
    end_filter:
        dstudio_free(path_filename);
        return ret;
}
