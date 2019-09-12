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

#define DSTUDIO_SET_TEXT_SCALE_MATRIX(matrix, width, height) \
    matrix[0].x = ((float) width / (float) DSTUDIO_VIEWPORT_WIDTH) / DSTUDIO_CHAR_SIZE_DIVISOR; \
    matrix[0].y = 0; \
    matrix[1].x = 0; \
    matrix[1].y = ((float) height / (float) DSTUDIO_VIEWPORT_HEIGHT) / DSTUDIO_CHAR_SIZE_DIVISOR;

typedef struct UITextSettingParams {
    unsigned int string_size;
    Vec2 * scale_matrix;
    GLfloat gl_x;
    GLfloat gl_y;
} UITextSettingParams;

void configure_text_element(UIElements * ui_text, void * params);
void update_text(UIElements * text, char * string_value, unsigned int string_size);

#endif
