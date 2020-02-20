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

#include "text.h"
#include "ui.h"


static void (*s_cancel_callback)(UIElements * ui_elements) = 0;
static void (*s_select_callback)(FILE * file_fd) = 0;
static UIElements * s_menu_background;
static Vec2 s_open_file_prompt_box_scale_matrix[2] = {0};
static Vec2 s_open_file_list_box_scale_matrix[2] = {0};
static UIElements * s_ui_elements;

static void close_open_file_menu() {
    configure_input(0);
    set_prime_interface(1);
    set_ui_elements_visibility(s_menu_background, 0, 1);
    set_ui_elements_visibility(s_ui_elements, 0, 3);
    if (s_cancel_callback) {
        s_cancel_callback(NULL);
    }
}

void init_open_menu(
    UIElements * menu_background,
    UIElements * ui_elements
) {
    s_ui_elements = ui_elements;
    s_menu_background = menu_background;
    UIElements * prompt_box  = ui_elements;
    UIElements * prompt = &ui_elements[1];
    UIElements * list_box = &ui_elements[2];
    
    s_open_file_prompt_box_scale_matrix[0].x = 1;
    s_open_file_prompt_box_scale_matrix[1].y = ((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_BOX_AREA_HEIGHT / (GLfloat) g_dstudio_viewport_height);
    
    prompt_box->color.r = 0;
    prompt_box->color.g = 0;
    prompt_box->color.b = 0;
    prompt_box->color.a = 0.5;
    
    init_ui_elements(
        prompt_box,
        NULL,
        &s_open_file_prompt_box_scale_matrix[0],
        0,
        1.0 - ( ((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_BOX_ABS_POS_Y) / g_dstudio_viewport_height),
        g_dstudio_viewport_width,
        DSTUDIO_OPEN_FILE_PROMPT_BOX_AREA_HEIGHT,
        0,
        0,
        1,
        1,
        10,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );

    init_ui_elements(
        prompt,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        -1.0 + (((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_ABS_POS_X) / g_dstudio_viewport_width),
        1.0 - (((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_ABS_POS_Y) / g_dstudio_viewport_height),
        DSTUDIO_OPEN_FILE_PROMPT_AREA_WIDTH,
        DSTUDIO_OPEN_FILE_PROMPT_AREA_HEIGHT,
        0,
        0,
        DSTUDIO_OPEN_FILE_PROMPT_COLUMN,
        DSTUDIO_OPEN_FILE_PROMPT_COUNT,
        DSTUDIO_OPEN_FILE_PROMPT_BUFFER_SIZE,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT,
        DSTUDIO_FLAG_NONE
    );
    
    update_text(prompt, "OPEN FILE", 9);
    prompt->request_render = 0;

    s_open_file_list_box_scale_matrix[0].x = 1;
    s_open_file_list_box_scale_matrix[1].y = ((GLfloat) g_dstudio_viewport_height - 76 )/ (GLfloat) g_dstudio_viewport_height;
    
    list_box->color.r = 0;
    list_box->color.g = 0;
    list_box->color.b = 0;
    list_box->color.a = 0.5;
    
    init_ui_elements(
        list_box,
        NULL,
        &s_open_file_list_box_scale_matrix[0],
        0,
        0,//1.0 - ( ((GLfloat) g_dstudio_viewport_height - 32) / g_dstudio_viewport_height) * 5,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height,
        0,
        0,
        1,
        1,
        10,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );
}

void open_file_menu(
    void (*cancel_callback)(UIElements * ui_elements),
    void (*select_callback)(FILE * file_fd)
) {
    s_cancel_callback = cancel_callback;
    s_select_callback = select_callback;
    configure_input(PointerMotionMask);
    set_prime_interface(0);
    set_ui_elements_visibility(s_menu_background, 1, 1);
    set_ui_elements_visibility(s_ui_elements, 1, 3);
    set_close_sub_menu_callback(close_open_file_menu);
    g_menu_background_enabled = s_menu_background;
}
