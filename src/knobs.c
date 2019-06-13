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
    glDeleteBuffers(1, &knobs->index_buffer_object);
    glDeleteBuffers(1, &knobs->vertex_buffer_object);
    glDeleteVertexArrays(1, &knobs->vertex_array_object);
    glDeleteTextures(1, &knobs->texture_id);
    free(knobs->instance_offsets_buffer);
    free(knobs->texture);
}

void finalize_knobs(UIKnobs * knobs, GLuint program_id) {
    glGenBuffers(1, &knobs->instance_offsets);
    glBindBuffer(GL_ARRAY_BUFFER, knobs->instance_offsets);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * knobs->count, knobs->instance_offsets_buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &knobs->instance_rotations);
    glBindBuffer(GL_ARRAY_BUFFER, knobs->instance_rotations);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * knobs->count, knobs->instance_rotations_buffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &knobs->vertex_array_object);
    glBindVertexArray(knobs->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, knobs->vertex_buffer_object);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribDivisor(0, 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            glVertexAttribDivisor(1, 0);
        glBindBuffer(GL_ARRAY_BUFFER, knobs->instance_offsets);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (GLvoid *) 0 );
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
        glBindBuffer(GL_ARRAY_BUFFER, knobs->instance_rotations);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *) 0 );
            glEnableVertexAttribArray(3);
            glVertexAttribDivisor(3, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init_knob(UIKnobs * knobs, int index, float offset_x, float offset_y) {    
    Vec2 * instance_offset = &knobs->instance_offsets_buffer[index];
    instance_offset->x = offset_x;
    instance_offset->y = offset_y;
    
    knobs->instance_rotations_buffer[index] = 0;
}

void init_knobs_cpu_side(UIKnobs * knobs, int count, GLuint texture_scale, const char * texture_filename) {
    knobs->count = count;
    
    knobs->instance_offsets_buffer = malloc(count * sizeof(Vec2));
    knobs->instance_rotations_buffer = malloc(count * sizeof(GLfloat));

    knobs->texture_scale = texture_scale;
    get_png_pixel(texture_filename, &knobs->texture, 1);
    
    GLchar * vertex_indexes = knobs->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    
    knobs->scale_matrix[0].x = ((float) texture_scale) / ((float) DSANDGRAINS_VIEWPORT_WIDTH);
    knobs->scale_matrix[0].y = 0;
    knobs->scale_matrix[1].x = 0;
    knobs->scale_matrix[1].y = ((float) texture_scale) / ((float) DSANDGRAINS_VIEWPORT_HEIGHT);
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
