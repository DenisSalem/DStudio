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

#ifndef DSTUDIO_TEXT_H_INCLUDED
#define DSTUDIO_TEXT_H_INCLUDED

#include "ui.h"

#define DSTUDIO_CHAR_SIZE_DIVISOR 13.0

//~ typedef struct UIText_t {
    //~ Vec4            vertex_attributes[4];
    //~ unsigned int    string_size;
    //~ unsigned int    actual_string_size;
    //~ Vec4 *          instance_offsets_buffer;
    //~ GLuint          instance_offsets;
    //~ Vec2            text_position;
    //~ GLuint          vertex_buffer_object;
    //~ GLuint          vertex_array_object;
    //~ GLchar          vertex_indexes[4];
    //~ GLuint          texture_id;
    //~ GLuint          index_buffer_object;
    //~ char *          string_buffer;
//~ } UIText;

typedef struct UITextSettingParams {
    unsigned int string_size;
    Vec2 * scale_matrix;
    GLfloat gl_x;
    GLfloat gl_y;
} UITextSettingParams;

void configure_text_element(UIElements * ui_text, void * params);
//void init_text(UIText * ui_text, int enable_aa, unsigned int string_size, const char * texture_filename, GLfloat pos_x, GLfloat pos_y, Vec2 * scale_matrix);
//void render_text(UIText * text);
void update_text(UIElements * text);
#endif
