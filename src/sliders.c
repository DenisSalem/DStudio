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

#include "sliders.h"

void finalize_sliders(UISliders * sliders) {
    finalize_ui_element(
        sliders->count,
        &sliders->instance_offsets,
        sliders->instance_offsets_buffer,
        &sliders->instance_translations,
        sliders->instance_translations_buffer,
        &sliders->vertex_array_object,
        sliders->vertex_buffer_object
    );
}

void init_slider(UISliders * sliders, int index, float offset_x, float offset_y) {    
    init_ui_element(
        &sliders->instance_offsets_buffer[index],
        offset_x,
        offset_y,
        &sliders->instance_translations_buffer[index]
    );
}

void init_sliders_cpu_side(UISliders * sliders, int count, GLuint texture_scale, const char * texture_filename, int viewport_width, int viewport_height) {
    init_ui_elements_cpu_side(
        count,
        &sliders->count,
        texture_scale,
        &sliders->texture_scale,
        texture_filename,
        &sliders->texture,
        &sliders->instance_offsets_buffer,
        &sliders->instance_translations_buffer,
        &sliders->vertex_indexes[0],
        &sliders->scale_matrix[0],
        viewport_width,
        viewport_height
    );
}

void init_sliders_gpu_side(UISliders * sliders) {
    init_ui_elements_gpu_side(
        0,
        sliders->vertexes_attributes,
        &sliders->vertex_buffer_object,
        &sliders->texture_id,
        sliders->texture_scale,
        sliders->texture_scale,
        sliders->texture,
        &sliders->index_buffer_object,
        sliders->vertex_indexes
    );
}

void render_sliders(UISliders * sliders) {    
    render_ui_elements(sliders->texture_id, sliders->vertex_array_object, sliders->index_buffer_object, sliders->count);
}

void update_slider(int index, void * context, void * args) {
    float * translation = (float*) args;
    UISliders * sliders_p = (UISliders *) context;
    sliders_p->instance_translations_buffer[index] = *translation;
    glBindBuffer(GL_ARRAY_BUFFER, sliders_p->instance_translations);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * sliders_p->count, sliders_p->instance_translations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
