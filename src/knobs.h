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

#ifndef DSTUDIO_KNOBS_H_INCLUDED
#define DSTUDIO_KNOBS_H_INCLUDED

#include "ui.h"

typedef struct UIKnobs_t {
    unsigned char *             texture;
    Vec4                        vertexes_attributes[4];
    Vec2                        scale_matrix[2];
    int                         count;
    Vec2  *                     instance_offsets_buffer;
    GLuint                      instance_offsets;
    GLfloat  *                  instance_rotations_buffer;
    GLuint                      instance_rotations;
    GLuint                      vertex_buffer_object;
    GLuint                      vertex_array_object;
    GLchar                      vertex_indexes[4];
    GLuint                      texture_id;
    GLuint                      texture_scale;
    GLuint                      index_buffer_object;
} UIKnobs;

void finalize_knobs(UIKnobs * knobs);
void init_knob(UIKnobs * knobs, int index, float x, float y);
void init_knobs_cpu_side(UIKnobs * knobs, int count, GLuint texture_scale, const char * texture_filename, int viewport_width, int viewport_height);
void init_knobs_gpu_side(UIKnobs * knobs);
void render_knobs(UIKnobs * knobs);
void update_knob(int index, void * context, void * args);

#define DSTUDIO_INIT_KNOB(knobs_p, knob_index, gl_x, gl_y, ui_element_index, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type) \
        init_knob( \
            knobs_p, \
            knob_index, \
            init_knob_array_p->gl_x, \
            init_knob_array_p->gl_y \
        ); \
        DSTUDIO_SET_AREA( \
            ui_element_index, \
            init_knob_array_p->min_area_x, \
            init_knob_array_p->max_area_x, \
            init_knob_array_p->min_area_y, \
            init_knob_array_p->max_area_y \
        ) \
        DSTUDIO_SET_UI_CALLBACK( \
            ui_element_index,\
            update_knob, \
            knob_index,\
            knobs_p, \
            init_knob_array_p->ui_element_type\
        );

#endif
