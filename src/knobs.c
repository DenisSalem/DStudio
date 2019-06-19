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
#include "dsandgrains/ui.h"
#include "knobs.h"

void free_knobs(UIKnobs * knobs) {
    free(knobs->instance_offsets_buffer);
    free(knobs->texture);
}

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

void init_knobs_cpu_side(UIKnobs * knobs, int count, GLuint texture_scale, const char * texture_filename) {
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
        DSANDGRAINS_VIEWPORT_WIDTH,
        DSANDGRAINS_VIEWPORT_HEIGHT
    );
}

void init_knobs_gpu_side(UIKnobs * knobs) {
    Vec4 * vertexes_attributes = knobs->vertexes_attributes;
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES

    GLuint * vertex_buffer_object_p = &knobs->vertex_buffer_object;
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &knobs->texture_id);
    glBindTexture(GL_TEXTURE_2D, knobs->texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, knobs->texture_scale, knobs->texture_scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, knobs->texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenBuffers(1, &knobs->index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLchar), knobs->vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render_knobs(UIKnobs * knobs) {    
    glBindTexture(GL_TEXTURE_2D, knobs->texture_id);
        glBindVertexArray(knobs->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, knobs->count);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void update_knob(int index, void * context, void * args) {
    float * rotation = (float*) args;
    UIKnobs * sample_knobs_p = (UIKnobs *) context;
    sample_knobs_p->instance_rotations_buffer[index] = *rotation;
    glBindBuffer(GL_ARRAY_BUFFER, sample_knobs_p->instance_rotations);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * sample_knobs_p->count, sample_knobs_p->instance_rotations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
