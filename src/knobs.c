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

#include <stdlib.h>

#include "extensions.h"
#include "knobs.h"

void finalize_knobs(UIKnobs * knobs) {
    finalize_ui_element(
        knobs->count,
        &knobs->instance_offsets,
        knobs->instance_offsets_buffer,
        &knobs->instance_rotations,
        knobs->instance_rotations_buffer,
        &knobs->vertex_array_object,
        knobs->vertex_buffer_object
    );
}

void init_knob(UIKnobs * knobs, int index, float offset_x, float offset_y) {    
    init_ui_element(&knobs->instance_offsets_buffer[index], offset_x, offset_y, &knobs->instance_rotations_buffer[index]);
}

void init_knobs_cpu_side(UIKnobs * knobs, int count, GLuint texture_scale, const char * texture_filename, int viewport_width, int viewport_height) {
    init_ui_elements_cpu_side(
        count,
        &knobs->count,
        texture_scale,
        &knobs->texture_scale,
        texture_filename,
        &knobs->texture,
        &knobs->instance_offsets_buffer,
        &knobs->instance_rotations_buffer,
        &knobs->vertex_indexes[0],
        &knobs->scale_matrix[0],
        viewport_width,
        viewport_height
    );
}

void init_knobs_gpu_side(UIKnobs * knobs) {
    init_ui_elements_gpu_side(
        1,
        knobs->vertex_attributes,
        &knobs->vertex_buffer_object,
        &knobs->texture_id,
        knobs->texture_scale,
        knobs->texture_scale,
        knobs->texture,
        &knobs->index_buffer_object,
        knobs->vertex_indexes
    );
}

void render_knobs(UIKnobs * knobs) {    
    render_ui_elements(knobs->texture_id, knobs->vertex_array_object, knobs->index_buffer_object, knobs->count);
}

void update_knob(int index, void * context, void * args) {
    float * rotation = (float*) args;
    UIKnobs * knobs_p = (UIKnobs *) context;
    knobs_p->instance_rotations_buffer[index] = *rotation;
    glBindBuffer(GL_ARRAY_BUFFER, knobs_p->instance_rotations);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * knobs_p->count, knobs_p->instance_rotations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
