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

#include "ui.h"

typedef struct Knob_t {
    float rotation;
    VertexAttributes vertexes_attributes[4];
    GLuint vertex_buffer_object;
} Knob;

typedef struct Knobs_t {
    unsigned char * image_base;
    unsigned char * image_cursor;
    int             size_image_base;
    int             size_image_cursor;
    Knob *          items;
    GLuint          vertex_indexes[6];
    GLuint          texture_image_base_id;
    GLuint          texture_image_cursor_id;
    GLuint          texture_width;
    GLuint          texture_height;
    GLuint          program_id;
} Knobs;

void init_knob(Knobs * knobs, int index, float width, float height, float x, float y);
void init_knobs(Knobs * knobs, int count, GLuint texture_width, GLuint texture_height, const char * image_base, const char * image_cursor);
void free_knobs(Knobs * knobs);
