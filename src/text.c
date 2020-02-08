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

#include <string.h>
#include "extensions.h"
#include "text.h"

void update_text(UIElements * text, char * string_value, unsigned int buffer_size) {
        Vec4 * offset_buffer = (Vec4 *) text->instance_offsets_buffer;
        int linear_coordinate = 0;
        int padding = 0;
        int request_update = 0;
        for (unsigned int i = 0; i < buffer_size; i++) {
            if (padding || string_value[i] == 0) {
                if (offset_buffer[i].z || offset_buffer[i].w) {
                    request_update = 1;
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
            if (request_update || z != offset_buffer[i].z || w != offset_buffer[i].w) {
                offset_buffer[i].z = z;
                offset_buffer[i].w = w;
                request_update = 1;
            }
        }
        
        if(request_update) {
            glBindBuffer(GL_ARRAY_BUFFER, text->instance_offsets);
                glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size * sizeof(Vec4), offset_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            text->request_render = 1;
            if (text->type == DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM) {
                text->interactive_list->highlight->request_render = 1;
            }
        }
}
