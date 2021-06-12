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

#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "buttons.h"
#include "extensions.h"
#include "window_management.h"

uint_fast32_t   g_dstudio_buttons_count = 0;
UIElements **   g_dstudio_buttons_register = NULL;

void dstudio_register_buttons() {
    uint_fast32_t mask  = DSTUDIO_UI_ELEMENT_TYPE_BUTTON | DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE;
    uint_fast32_t index = 0;
    g_dstudio_buttons_register = dstudio_alloc(sizeof(UIElements *)*g_dstudio_buttons_count, DSTUDIO_FAILURE_IS_FATAL);
    
    for (uint_fast32_t i = 0; i < g_dstudio_ui_element_count; i++) {
        if (g_dstudio_ui_elements_array[i].type & mask) {
            g_dstudio_buttons_register[index++] = &g_dstudio_ui_elements_array[i];
        }
    }
};

void dstudio_update_ui_bouncing_buttons() {
    double timestamp = 0;
    double elapsed_time = 0;
    int pointer_x, pointer_y;
    uint_fast32_t texture_index;
    
    for (uint_fast32_t i = 0; i < g_dstudio_buttons_count; i++) {
        if (g_dstudio_buttons_register[i]->type == DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE) {
            timestamp = g_dstudio_buttons_register[i]->timestamp;
            if (timestamp == 0) {
                continue;
            }
            elapsed_time = dstudio_get_timestamp() - timestamp;
    
            if (elapsed_time > 0.05) {
                if (g_dstudio_buttons_register[i]->texture_index == 0 && g_dstudio_mouse_state == 0) {
                    g_dstudio_buttons_register[i]->timestamp = 0;
                    continue;
                }
                if (g_dstudio_buttons_register[i]->texture_index == 1 && g_dstudio_mouse_state == 0 && g_dstudio_buttons_register[i]->render_state == DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED) {
                    dstudio_update_button(g_dstudio_buttons_register[i]);
                }
            }
        }
        else if (
            g_dstudio_buttons_register[i]->enabled && 
            (g_x11_input_mask & PointerMotionMask)
        ) {
            get_pointer_coordinates(&pointer_x, &pointer_y);
            if (pointer_x >= (g_scissor_offset_x + g_dstudio_buttons_register[i]->areas.min_area_x) && pointer_x <= (g_scissor_offset_x + g_dstudio_buttons_register[i]->areas.max_area_x) &&
                pointer_y >= (g_scissor_offset_y + g_dstudio_buttons_register[i]->areas.min_area_y) && pointer_y <= (g_scissor_offset_y + g_dstudio_buttons_register[i]->areas.max_area_y)) {
                texture_index = 1;
            }
            else {
                texture_index = 0;
            }
    
            if (texture_index != g_dstudio_buttons_register[i]->texture_index) {
                dstudio_update_button(g_dstudio_buttons_register[i]);
            }
        }
    }
}
 
void dstudio_update_button(UIElements * buttons_p) {
    if (buttons_p->texture_index == 1) {
        buttons_p->texture_index = 0;
    }
    else {
        buttons_p->texture_index = 1;
    }
    buttons_p->timestamp = dstudio_get_timestamp();
    buttons_p->render_state = DSTUDIO_UI_ELEMENT_RENDER_REQUESTED;
}
