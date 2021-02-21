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

#include <string.h>
#include "extensions.h"
#include "text.h"


/* TODO : The whole mutex droping might need to completely update the way
 * buffer update is done before rendering for this ui element type.
 */
void update_text(UIElements * text, char * string_value, unsigned int buffer_size) {
    Vec4 * offset_buffer = (Vec4 *) text->instance_offsets_buffer;
    int linear_coordinate = 0;
    int padding = 0;
    size_t current_strlen = strlen(string_value);
    text->scissor.width = text->scale_matrix[0].x * g_dstudio_viewport_width;
    text->scissor.width *= current_strlen > text->previous_text_size ? \
        current_strlen: \
        text->previous_text_size;
    text->previous_text_size = current_strlen;

    //DSTUDIO_TRACE_ARGS("%s\n", string_value)

    for (unsigned int i = 0; i < buffer_size; i++) {
        if (padding || string_value[i] == 0) {
            if (offset_buffer[i].z || offset_buffer[i].w) {
                text->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
            }
            offset_buffer[i].z = 0;
            offset_buffer[i].w = 0;
            padding = 1;
            continue;
        }
        if (string_value[i] >= 32 && string_value[i] <= 126) {
            linear_coordinate = string_value[i] - 32;
        }
        GLfloat z = (GLfloat) (linear_coordinate % (int) DSTUDIO_CHAR_SIZE_DIVISOR) * (1.0 / DSTUDIO_CHAR_SIZE_DIVISOR);
        GLfloat w = (linear_coordinate / (int) DSTUDIO_CHAR_SIZE_DIVISOR) * (1.0 / DSTUDIO_CHAR_SIZE_DIVISOR);
        if (text->render_state == DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED || z != offset_buffer[i].z || w != offset_buffer[i].w) {
            offset_buffer[i].z = z;
            offset_buffer[i].w = w;
            text->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
        }
    }
    
    if(text->render_state == DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED) {
        text->text_buffer_size = buffer_size;
        if (text->type == DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM) {
            text->interactive_list->highlight->render_state = DSTUDIO_UI_ELEMENT_RENDER_REQUESTED;
        }
    }
}
