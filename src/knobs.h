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
    unsigned char *             texture;
    Vec4                        vertexes_attributes[4];
    int                         count;
    Vec2  *                     instance_offsets_buffer;
    GLint                       instance_offsets;
    GLfloat  *                  instance_rotations_buffer;
    GLint                       instance_rotations;
    GLuint                      vertex_buffer_object;
    GLuint                      vertex_array_object;
    GLchar                      vertex_indexes[4];
    GLuint                      texture_id;
    GLuint                      texture_scale;
    GLuint                      index_buffer_object;
} UiKnobs;

void free_knobs(UiKnobs * knobs);
void finalize_knobs(UiKnobs * knobs);
void init_knob(UiKnobs * knobs, int index, float x, float y);
void init_knobs(UiKnobs * knobs, int count, GLuint texture_scale, const char * texture_filename);
void render_knobs(UiKnobs * knobs);
