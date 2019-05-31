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

void free_knobs(UiKnobs * knobs) {
    glDeleteBuffers(1, &knobs->index_buffer_object);
    glDeleteBuffers(1, &knobs->vertex_buffer_object);
    glDeleteVertexArrays(1, &knobs->vertex_array_object);
    glDeleteTextures(1, &knobs->texture_id);
    free(knobs->instance_offsets_buffer);
    free(knobs->texture);
}

void finalize_knobs(UiKnobs * knobs) {
    glGenBuffers(1, &knobs->instance_offsets);
    glBindBuffer(GL_ARRAY_BUFFER, knobs->instance_offsets);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * knobs->count, knobs->instance_offsets_buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    knobs->instance_rotations_buffer[0] = 0;
    knobs->instance_rotations_buffer[1] = 0.1*3;
    knobs->instance_rotations_buffer[2] = 0.2*3;
    knobs->instance_rotations_buffer[3] = 0.3*3;

    knobs->instance_rotations_buffer[4] = 0.4*3;
    knobs->instance_rotations_buffer[5] = 0.5*3;
    knobs->instance_rotations_buffer[6] = 0.6*3;
    knobs->instance_rotations_buffer[7] = 0.7*3;

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

void init_knob(UiKnobs * knobs, int index, float offset_x, float offset_y) {    
    Vec2 * instance_offset = &knobs->instance_offsets_buffer[index];
    instance_offset->x = offset_x;
    instance_offset->y = offset_y;
    
    knobs->instance_rotations_buffer[index] = 0;
}

void init_knobs(UiKnobs * knobs, int count, GLuint texture_scale, const char * texture_filename) {
    knobs->count = count;
    knobs->instance_offsets_buffer = malloc(count * sizeof(Vec2));
    knobs->instance_rotations_buffer = malloc(count * sizeof(GLfloat));
    knobs->texture_scale = texture_scale;
    get_png_pixel(texture_filename, &knobs->texture);
    
    GLchar * vertex_indexes = knobs->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    
    GLfloat width  = ((float) texture_scale) / (DSANDGRAINS_VIEWPORT_WIDTH >> 1);
    GLfloat height = ((float) texture_scale) / (DSANDGRAINS_VIEWPORT_HEIGHT >> 1);
    
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_scale, texture_scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, knobs->texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenBuffers(1, &knobs->index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLchar), knobs->vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render_knobs(UiKnobs * knobs) {
    glBindTexture(GL_TEXTURE_2D, knobs->texture_id);
        glBindVertexArray(knobs->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, 8);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
