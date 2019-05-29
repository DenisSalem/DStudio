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
    glDeleteTextures(1, &knobs->texture_image_base_id);
    glDeleteTextures(1, &knobs->texture_image_cursor_id);
    free(knobs->base_instance_transformations_buffer);
    free(knobs->cursor_instance_transformations_buffer);
    free(knobs->image_base);
    free(knobs->image_cursor);
}

void init_knob(UiKnobs * knobs, int index, float scale, float x, float y) {
}

void init_knobs(UiKnobs * knobs, int count, GLuint texture_scale, const char * image_base, const char * image_cursor) {
    knobs->base_instance_transformations_buffer = malloc(count * sizeof(InstanceTransformation));
    knobs->cursor_instance_transformations_buffer = malloc(count * sizeof(InstanceTransformation));
    knobs->texture_scale = texture_scale;
    get_png_pixel(image_base, &knobs->image_base);
    get_png_pixel(image_cursor, &knobs->image_cursor);
    
    GLchar * vertex_indexes = knobs->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    
    GLfloat width  = ((float) texture_scale) / (DSANDGRAINS_VIEWPORT_WIDTH >> 1);
    GLfloat height = ((float) texture_scale) / (DSANDGRAINS_VIEWPORT_HEIGHT >> 1);
    
    VertexAttributes * vertexes_attributes = knobs->vertexes_attributes;
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES

    GLuint * vertex_buffer_object_p = &knobs->vertex_buffer_object;
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttributes) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &knobs->texture_image_base_id);
    glBindTexture(GL_TEXTURE_2D, knobs->texture_image_base_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_scale, texture_scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, knobs->image_base);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenTextures(1, &knobs->texture_image_cursor_id);
    glBindTexture(GL_TEXTURE_2D, knobs->texture_image_cursor_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_scale, texture_scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, knobs->image_cursor);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenBuffers(1, &knobs->index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLchar), knobs->vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &knobs->vertex_array_object);
    glBindVertexArray(knobs->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, knobs->vertex_buffer_object);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //~ InstanceTransformation * base_instance_transformations_buffer = knobs->base_instance_transformations_buffer;
    
    //~ base_instance_transformations_buffer[0].offset_x = 0;
    //~ base_instance_transformations_buffer[0].offset_y = 0;
    //~ base_instance_transformations_buffer[0].rotation = 0;
    
    //~ base_instance_transformations_buffer[1].offset_x = width;
    //~ base_instance_transformations_buffer[1].offset_y = 0;
    //~ base_instance_transformations_buffer[1].rotation = 0;
    
    //~ base_instance_transformations_buffer[2].offset_x = width * 2;
    //~ base_instance_transformations_buffer[2].offset_y = 0;
    //~ base_instance_transformations_buffer[2].rotation = 0;

    //~ base_instance_transformations_buffer[3].offset_x = width * 3;
    //~ base_instance_transformations_buffer[3].offset_y = 0;
    //~ base_instance_transformations_buffer[3].rotation = 0;
    
    //~ glGenBuffers(1, &knobs->base_instance_transformations);
    //~ glBindBuffer(GL_ARRAY_BUFFER, knobs->base_instance_transformations);
    //~ glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceTransformation) * 8, &knobs->base_instance_transformations_buffer, GL_STATIC_DRAW);
    //~ glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void render_knobs(UiKnobs * knobs) {
    glBindTexture(GL_TEXTURE_2D, knobs->texture_image_base_id);
        glBindVertexArray(knobs->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
                glDrawElements(GL_TRIANGLE_STRIP, 4,  GL_UNSIGNED_BYTE, (GLvoid *) 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindTexture(GL_TEXTURE_2D, knobs->texture_image_cursor_id);
        glBindVertexArray(knobs->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knobs->index_buffer_object);
                glDrawElements(GL_TRIANGLE_STRIP, 4,  GL_UNSIGNED_BYTE, (GLvoid *) 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
