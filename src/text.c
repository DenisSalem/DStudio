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

#include "text.h"

void init_text(UIText * ui_text, const char * texture_filename, unsigned int texture_width, unsigned int texture_height, unsigned int viewport_width,unsigned int viewport_height) {
    png_bytep texture_buffer;
    get_png_pixel(texture_filename, &texture_buffer, PNG_FORMAT_RGBA);

    GLchar * vertex_indexes = ui_text->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    
    Vec4 * vertex_attributes = ui_text->vertex_attributes;
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES

    Vec2 * scale_matrix = ui_text->scale_matrix;
    scale_matrix[0].x = ((float) texture_width / DSTUDIO_CHAR_SIZE_DIVISOR) / ((float) viewport_width);
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_height / DSTUDIO_CHAR_SIZE_DIVISOR) / ((float) viewport_height);
}
