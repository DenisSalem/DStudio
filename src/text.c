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

void init_text(UIText * ui_text, int enable_aa, unsigned int string_size, const char * texture_filename, GLfloat pos_x, GLfloat pos_y, Vec2 * input_scale_matrix) {
    Vec2 * scale_matrix = NULL;
    png_bytep texture_buffer;
    get_png_pixel(texture_filename, &texture_buffer, PNG_FORMAT_RGBA);
    ui_text->string_size = string_size;
    ui_text->actual_string_size = 0;
    ui_text->string_buffer = malloc(sizeof(char) * string_size);
    explicit_bzero(ui_text->string_buffer, sizeof(char) * string_size);
    
    int text_texture_width = DSTUDIO_CHAR_TABLE_ASSET_WIDTH;
    int text_texture_height = DSTUDIO_CHAR_TABLE_ASSET_HEIGHT;
    if (0 != strcmp(texture_filename, DSTUDIO_CHAR_TABLE_ASSET_PATH)) {
        text_texture_width /= 2;
        text_texture_height /= 2;
    }
    
    GLchar * vertex_indexes = ui_text->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    gen_gl_buffer(GL_ELEMENT_ARRAY_BUFFER, &ui_text->index_buffer_object, vertex_indexes, GL_STATIC_DRAW, sizeof(GLchar) * 4);

    Vec4 * vertex_attributes = ui_text->vertex_attributes;
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES(1.0f / (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR, 1.0f / (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR)
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_text->vertex_buffer_object, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);

    if (input_scale_matrix == NULL) {
        ui_text->scale_matrix = malloc(sizeof(Vec2) * 2);
        scale_matrix = ui_text->scale_matrix;
        scale_matrix[0].x = ((float) text_texture_width / (float) DSTUDIO_VIEWPORT_WIDTH) / DSTUDIO_CHAR_SIZE_DIVISOR;
        scale_matrix[0].y = 0;
        scale_matrix[1].x = 0;
        scale_matrix[1].y = ((float) text_texture_height / (float) DSTUDIO_VIEWPORT_HEIGHT) / DSTUDIO_CHAR_SIZE_DIVISOR;
    }
    else {
        ui_text->scale_matrix = input_scale_matrix;
        scale_matrix = input_scale_matrix;
    }
    
    //~ setup_texture_gpu_side(
        //~ enable_aa,
        //~ 1,
        //~ &ui_text->texture_id,
        //~ text_texture_width,
        //~ text_texture_height,
        //~ texture_buffer
    //~ );
    
    ui_text->instance_offsets_buffer = malloc(sizeof(Vec4) * string_size);
    explicit_bzero(ui_text->instance_offsets_buffer, sizeof(Vec4) * string_size);
    for (int i=0; i < string_size; i++) {
        ui_text->instance_offsets_buffer[i].x = pos_x + i * scale_matrix[0].x * 2;
        ui_text->instance_offsets_buffer[i].y = pos_y;
    }
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_text->instance_offsets, ui_text->instance_offsets_buffer, GL_STATIC_DRAW, sizeof(Vec4) * string_size);
    //setup_vertex_array_gpu_side(&ui_text->vertex_array_object, ui_text->vertex_buffer_object, ui_text->instance_offsets, 0);
}

void render_text(UIText * text) {
    if (text->actual_string_size) {
        //render_ui_elements(text->texture_id, text->vertex_array_object, text->index_buffer_object, text->actual_string_size);
    }
}

void update_text(UIText * text) {
        char * string_value = text->string_buffer;
        Vec4 * offset_buffer = text->instance_offsets_buffer;
        int linear_coordinate, coordinate_x, coordinate_y;
        
        for (int i = 0; string_value[i] != 0; i++) {
            if (string_value[i] >= 32 && string_value[i] <= 126) {
                linear_coordinate = string_value[i] - 32;
            }
            offset_buffer[i].z = (GLfloat) (linear_coordinate % (int) DSTUDIO_CHAR_SIZE_DIVISOR) * (1.0 / DSTUDIO_CHAR_SIZE_DIVISOR);
            offset_buffer[i].w = (linear_coordinate / (int) DSTUDIO_CHAR_SIZE_DIVISOR) * (1.0 / DSTUDIO_CHAR_SIZE_DIVISOR);

        }
        text->actual_string_size = strlen(string_value);
        glBindBuffer(GL_ARRAY_BUFFER, text->instance_offsets);
            glBufferSubData(GL_ARRAY_BUFFER, 0, text->actual_string_size * sizeof(Vec4), offset_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}
