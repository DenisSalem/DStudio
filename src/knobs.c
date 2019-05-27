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

void init_knobs(Knobs * knobs, int count, GLuint texture_width, GLuint texture_height, const char * image_base, const char * image_cursor) {
    knobs->items = malloc(count * sizeof(Knob));
    knobs->texture_width = texture_width;
    knobs->texture_height = texture_height;
    knobs->size_image_base = get_png_pixel(image_base, &knobs->image_base);
    knobs->size_image_cursor = get_png_pixel(image_cursor, &knobs->image_cursor);
    
    GLuint * vertex_indexes = knobs->vertex_indexes;
    vertex_indexes[0] = 0;
    vertex_indexes[1] = 1;
    vertex_indexes[2] = 2;
    vertex_indexes[3] = 3;
}

void free_knobs(Knobs * knobs) {
    free(knobs->items);
    free(knobs->image_base);
    free(knobs->image_cursor);
}

void init_knob(Knobs * knobs, int index, float width, float height, float x, float y) {
    VertexAttributes * vertexes_attributes = knobs->items[index].vertexes_attributes;
    GLuint * vertex_buffer_object_p = &knobs->items[index].vertex_buffer_object;

    vertexes_attributes[0].x = -1.0f;
    vertexes_attributes[0].y =  1.0f;
    vertexes_attributes[1].x = -1.0f;
    vertexes_attributes[1].y = -1.0f;
    vertexes_attributes[2].x =  1.0f;
    vertexes_attributes[2].y =  1.0f;   
    vertexes_attributes[3].x =  1.0f;
    vertexes_attributes[3].y = -1.0f;
    
    vertexes_attributes[0].s = 0.0f;
    vertexes_attributes[0].t = 0.0f;
    vertexes_attributes[1].s = 0.0f;
    vertexes_attributes[1].t = 1.0f;
    vertexes_attributes[2].s = 1.0f;
    vertexes_attributes[2].t = 0.0f;
    vertexes_attributes[3].s = 1.0f;
    vertexes_attributes[3].t = 1.0f;
    
    
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttributes) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
