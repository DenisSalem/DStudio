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

void configure_text_element(UIElements * ui_text, void * params) {
    Vec4 * vertex_attributes = ui_text->vertex_attributes;
    vertex_attributes[1].w /= (GLfloat)DSTUDIO_CHAR_SIZE_DIVISOR;
    vertex_attributes[2].z /= (GLfloat)DSTUDIO_CHAR_SIZE_DIVISOR;
    vertex_attributes[3].z /= (GLfloat)DSTUDIO_CHAR_SIZE_DIVISOR;
    vertex_attributes[3].w /= (GLfloat)DSTUDIO_CHAR_SIZE_DIVISOR;
    
    UITextSettingParams * ui_text_setting_params = (UITextSettingParams *) params;
    for (unsigned int i = 0; i < ui_text_setting_params->string_size; i++) {
        ((Vec4 *) ui_text->instance_offsets_buffer)[i].x = ui_text_setting_params->gl_x + i * ui_text_setting_params->scale_matrix[0].x * 2;
        ((Vec4 *) ui_text->instance_offsets_buffer)[i].y = ui_text_setting_params->gl_y;
    }
}

void update_text(UIElements * text, char * string_value, unsigned int string_size) {
        Vec4 * offset_buffer = (Vec4 *) text->instance_offsets_buffer;
        int linear_coordinate = 0;
        int padding = 0;
        for (unsigned int i = 0; i < string_size; i++) {
            if (string_value[i] == 0 || padding) {
                offset_buffer[i].z = 0;
                offset_buffer[i].w = 0;
                padding = 1;
                continue;
            }
            if (string_value[i] >= 32 && string_value[i] <= 126) {
                linear_coordinate = string_value[i] - 32;
            }
            offset_buffer[i].z = (GLfloat) (linear_coordinate % (int) DSTUDIO_CHAR_SIZE_DIVISOR) * (1.0 / DSTUDIO_CHAR_SIZE_DIVISOR);
            offset_buffer[i].w = (linear_coordinate / (int) DSTUDIO_CHAR_SIZE_DIVISOR) * (1.0 / DSTUDIO_CHAR_SIZE_DIVISOR);

        }
        glBindBuffer(GL_ARRAY_BUFFER, text->instance_offsets);
            glBufferSubData(GL_ARRAY_BUFFER, 0, string_size * sizeof(Vec4), offset_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}
