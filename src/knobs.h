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

typedef struct UiKnobs_t {
    unsigned char *             image_base;
    unsigned char *             image_cursor;
    VertexAttributes            vertexes_attributes[4];
    InstanceTransformation  *   base_instance_transformations_buffer;
    InstanceTransformation  *   cursor_instance_transformations_buffer;
    GLint                       base_instance_transformations;
    GLint                       cursor_instance_transformations;
    GLuint                      vertex_buffer_object;
    GLuint                      vertex_array_object;
    GLchar                      vertex_indexes[4];
    GLuint                      texture_image_base_id;
    GLuint                      texture_image_cursor_id;
    GLuint                      texture_scale;
    GLuint                      index_buffer_object;
} UiKnobs;

void free_knobs(UiKnobs * knobs);
void init_knob(UiKnobs * knobs, int index, float scale, float x, float y);
void init_knobs(UiKnobs * knobs, int count, GLuint texture_scale, const char * image_base, const char * image_cursor);
void render_knobs(UiKnobs * knobs);
