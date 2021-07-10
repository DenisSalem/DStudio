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

#include "dstudio.h"

static Vec2 s_text_pointer_4x9_scale_matrix[2] = {0};
static Vec2 s_text_pointer_8x18_scale_matrix[2] = {0};
static double s_timestamp = 0;

UITextPointerContext g_text_pointer_context = {0}; 

void clear_text_pointer() {
    if (g_text_pointer_context.active) {
        g_text_pointer_context.active = 0;
        *g_text_pointer_context.text_pointer->instance_alphas_buffer = 0.0;
        update_text_pointer();
    }
}

void compute_text_pointer_coordinates(uint_fast32_t index) {
    uint_fast32_t half_viewport_width = g_dstudio_viewport_width >> 1;
    uint_fast32_t half_viewport_height = g_dstudio_viewport_height >> 1;
    *g_text_pointer_context.text_pointer->instance_alphas_buffer = 1.0;

    // We need to compute coordinates in such way that the pointer will be perfectly aligned with pixels.
    GLfloat x_inc = 1.0 / (GLfloat) half_viewport_width;
    GLfloat y_inc = 1.0 / (GLfloat) half_viewport_height;
    int_fast32_t x_multiplier = g_text_pointer_context.ui_text->coordinates_settings.instance_offsets_buffer[index].x / x_inc;
    g_text_pointer_context.text_pointer->coordinates_settings.instance_offsets_buffer->x = (x_multiplier - (g_text_pointer_context.char_width) - 1) * x_inc;        
    g_text_pointer_context.text_pointer->coordinates_settings.instance_offsets_buffer->y = g_text_pointer_context.ui_text->coordinates_settings.instance_offsets_buffer[index].y + y_inc;
    
    // We compute the exact amount of pixel to render.
    g_text_pointer_context.text_pointer->coordinates_settings.scissor.x = half_viewport_width + half_viewport_width * g_text_pointer_context.text_pointer->coordinates_settings.instance_offsets_buffer->x - 1;
    g_text_pointer_context.text_pointer->coordinates_settings.scissor.y = half_viewport_height + half_viewport_height * g_text_pointer_context.text_pointer->coordinates_settings.instance_offsets_buffer->y - (g_text_pointer_context.text_pointer_height >>1 ) - 1;
    g_text_pointer_context.text_pointer->coordinates_settings.scissor.width = 1;
    g_text_pointer_context.text_pointer->coordinates_settings.scissor.height = g_text_pointer_context.text_pointer_height;
}

void init_ui_text_pointer(UIElements * text_pointer) {
    s_text_pointer_4x9_scale_matrix[0].x = (DSTUDIO_TEXT_POINTER_WIDTH / (float) g_dstudio_viewport_width);
    s_text_pointer_4x9_scale_matrix[1].y = ((float) DSTUDIO_TEXT_POINTER_1_HEIGHT / (float) g_dstudio_viewport_height);

    s_text_pointer_8x18_scale_matrix[0].x = (DSTUDIO_TEXT_POINTER_WIDTH / (float) g_dstudio_viewport_width);
    s_text_pointer_8x18_scale_matrix[1].y = ((float) DSTUDIO_TEXT_POINTER_2_HEIGHT / (float) g_dstudio_viewport_height);    
    text_pointer->color.r = 1;
    text_pointer->color.g = 0.5;
    text_pointer->color.b = 0;
    text_pointer->color.a = 1.0;

    init_ui_elements(
        text_pointer,
        NULL,
        &s_text_pointer_4x9_scale_matrix[0],
        0,
        0,
        DSTUDIO_TEXT_POINTER_WIDTH,
        DSTUDIO_TEXT_POINTER_1_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );

    text_pointer->render_state = DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED;
    
    g_text_pointer_context.text_pointer = text_pointer;
}

void update_text_pointer_context(UIElements * ui_elements) {
    UIInteractiveList * interactive_list = 0;

    switch(ui_elements->type) {
        case DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM:
            interactive_list = ui_elements->interactive_list;
            g_text_pointer_context.ui_text = ui_elements;
            g_text_pointer_context.highlight = interactive_list->highlight;
            for(uint_fast32_t i = 0; i < interactive_list->lines_number; i++) {
                if (&interactive_list->lines[i] == ui_elements) {
                    uint_fast32_t index = i+interactive_list->window_offset;
                    if (index >= *interactive_list->source_data_count) {
                        return;
                    }
                    g_text_pointer_context.string_buffer = &interactive_list->source_data[index*interactive_list->stride];
                    g_text_pointer_context.buffer_size = interactive_list->string_size;
                    g_text_pointer_context.text_pointer_height = (g_dstudio_viewport_height) * ui_elements->coordinates_settings.scale_matrix[1].y;
                    s_timestamp = dstudio_get_timestamp();
                    update_text_pointer();
                    break;
                }
            }
            uint_fast32_t last_char_index = strlen(g_text_pointer_context.string_buffer);
            g_text_pointer_context.insert_char_index = last_char_index;
            compute_text_pointer_coordinates(last_char_index);
            g_text_pointer_context.active = 1;
            break;
            
        default:
            g_text_pointer_context.active = 0;
            return;
    }


}

void text_pointer_blink() {
    double now = dstudio_get_timestamp();
    GLfloat * text_pointer_alphas_buffer = g_text_pointer_context.text_pointer->instance_alphas_buffer;

    if (now - s_timestamp < 0.5 || !g_text_pointer_context.active || !is_window_focus()) {
        return;
    }
    s_timestamp = now;
    if (*text_pointer_alphas_buffer == 1.0) {
        *text_pointer_alphas_buffer = 0.0;
    }
    else {
        *text_pointer_alphas_buffer = 1.0;
    }
    update_text_pointer();
}

void update_text_box(unsigned int keycode) {   
    uint_fast32_t string_size = strlen(g_text_pointer_context.string_buffer);
    char * string_buffer = g_text_pointer_context.string_buffer;
    GLfloat * text_pointer_alphas_buffer = g_text_pointer_context.text_pointer->instance_alphas_buffer;

    if (keycode == DSTUDIO_KEY_CODE_ERASEBACK) {
        if (g_text_pointer_context.insert_char_index == 0) {
            return;
        }
        
        if (g_text_pointer_context.insert_char_index == string_size) {
            string_buffer[--g_text_pointer_context.insert_char_index] = 0;
        }
        else {
            for (uint_fast32_t i = g_text_pointer_context.insert_char_index; i < string_size; i++) {
                string_buffer[i-1] = string_buffer[i];
            }
            string_buffer[string_size-1] = 0;
            g_text_pointer_context.insert_char_index--;
        }
    }
    else if (keycode == DSTUDIO_KEY_LEFT_ARROW){
        if (g_text_pointer_context.insert_char_index <= 0) {
            return;
        }
        g_text_pointer_context.insert_char_index--;
    }
    else if (keycode == DSTUDIO_KEY_RIGHT_ARROW){
        if (g_text_pointer_context.insert_char_index >= string_size) {
            return;
        }
        g_text_pointer_context.insert_char_index++;
    }
    else if (keycode >= 32 && keycode <= 126) {
        if (string_size + 1 >= g_text_pointer_context.buffer_size) {
            return;
        }
        if (string_size > 0) {
            for (uint_fast32_t i = string_size; i > g_text_pointer_context.insert_char_index; i--) {
                string_buffer[i] = string_buffer[i-1];
            }
        }
        string_buffer[g_text_pointer_context.insert_char_index++] = (char) keycode;
    }
    else {
        return;
    }
    *text_pointer_alphas_buffer = 1.0;
    compute_text_pointer_coordinates(g_text_pointer_context.insert_char_index);
    update_text_pointer();
    update_text(g_text_pointer_context.ui_text, string_buffer, g_text_pointer_context.buffer_size);   
    if (
        g_text_pointer_context.ui_text->type == DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM &&
        g_text_pointer_context.ui_text->interactive_list->edit_item_callback
    ) {
        g_text_pointer_context.ui_text->interactive_list->edit_item_callback(
            g_text_pointer_context.ui_text->interactive_list->index + \
            g_text_pointer_context.ui_text->interactive_list->window_offset
        );
    }
}

void update_text_pointer() {
    g_text_pointer_context.text_pointer->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
    if (g_text_pointer_context.highlight && g_text_pointer_context.ui_text->type == DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM) {
        g_text_pointer_context.highlight->render_state = DSTUDIO_UI_ELEMENT_RENDER_REQUESTED;
    }
    if (g_text_pointer_context.ui_text->render_state != DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED) {
        g_text_pointer_context.ui_text->render_state = DSTUDIO_UI_ELEMENT_RENDER_REQUESTED;
    }
}
