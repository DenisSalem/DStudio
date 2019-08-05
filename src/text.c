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

void init_text(UIText * ui_text, unsigned int string_size, const char * texture_filename, unsigned int texture_width, unsigned int texture_height, unsigned int viewport_width, unsigned int viewport_height, GLfloat pos_x, GLfloat pos_y) {
    png_bytep texture_buffer;
    get_png_pixel(texture_filename, &texture_buffer, PNG_FORMAT_RGBA);
    ui_text->string_size = string_size;
    ui_text->actual_string_size = 0;
    ui_text->string_buffer = malloc(sizeof(char) * string_size);

    explicit_bzero(ui_text->string_buffer, sizeof(char) * string_size);

    GLchar * vertex_indexes = ui_text->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    gen_gl_buffer(GL_ELEMENT_ARRAY_BUFFER, &ui_text->index_buffer_object, vertex_indexes, GL_STATIC_DRAW, sizeof(GLchar) * 4);

    Vec4 * vertex_attributes = ui_text->vertex_attributes;
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES(1.0f / (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR, 1.0f / (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR)
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_text->vertex_buffer_object, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);

    Vec2 * scale_matrix = ui_text->scale_matrix;
    scale_matrix[0].x = ((float) texture_width / (float) viewport_width) / DSTUDIO_CHAR_SIZE_DIVISOR;
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_height / (float) viewport_height) / DSTUDIO_CHAR_SIZE_DIVISOR;
        
    glGenTextures(1, &ui_text->texture_id);
    glBindTexture(GL_TEXTURE_2D, ui_text->texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    free(texture_buffer);
    
    ui_text->instance_offsets_buffer = malloc(sizeof(Vec4) * string_size);
    explicit_bzero(ui_text->instance_offsets_buffer, sizeof(Vec4) * string_size);
    for (int i=0; i < string_size; i++) {
        ui_text->instance_offsets_buffer[i].x = pos_x + i * scale_matrix[0].x * 2;
        ui_text->instance_offsets_buffer[i].y = pos_y;
        ui_text->instance_offsets_buffer[i].z = 6 / DSTUDIO_CHAR_SIZE_DIVISOR;
        ui_text->instance_offsets_buffer[i].w = 3 / DSTUDIO_CHAR_SIZE_DIVISOR;
    }
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_text->instance_offsets, ui_text->instance_offsets_buffer, GL_STATIC_DRAW, sizeof(Vec4) * string_size);
    
    glGenVertexArrays(1, &ui_text->vertex_array_object);
    glBindVertexArray(ui_text->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, ui_text->vertex_buffer_object);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribDivisor(0, 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            glVertexAttribDivisor(1, 0);
        glBindBuffer(GL_ARRAY_BUFFER, ui_text->instance_offsets);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), (GLvoid *) 0 );
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void render_text(UIText * text) {
    if (text->actual_string_size) {
        render_ui_elements(text->texture_id, text->vertex_array_object, text->index_buffer_object, text->actual_string_size);
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
