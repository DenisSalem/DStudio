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

#include <time.h>
#include <unistd.h>

#include "extensions.h"
#include "system_usage.h"

void init_system_usage_ui(UISystemUsage * system_usage, const char * texture_filename, int texture_width, int texture_height, int viewport_width, int viewport_height) {
    system_usage->texture_width = texture_width;
    system_usage->texture_height = texture_height;

    get_png_pixel(texture_filename, &system_usage->texture, PNG_FORMAT_RGBA);
    
    GLuint * vertex_indexes = system_usage->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    
    Vec4 * vertexes_attributes = system_usage->vertexes_attributes;
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES
    
    Vec2 * scale_matrix = system_usage->scale_matrix;
    scale_matrix[0].x = ((float) texture_width) / ((float) viewport_width);
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_height) / ((float) viewport_height);

    system_usage->index_buffer_object = 0;
    
    glGenBuffers(1, &system_usage->index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, system_usage->index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   
    GLuint * vertex_buffer_object_p = &system_usage->vertex_buffer_object;
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenTextures(1, &system_usage->texture_id);
    glBindTexture(GL_TEXTURE_2D, system_usage->texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, system_usage->texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &system_usage->vertex_array_object);
    glBindVertexArray(system_usage->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, system_usage->vertex_buffer_object);         
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void * update_system_usage(void * args) {
    SystemUsage * system_usage = (SystemUsage *) args;
    while (!system_usage->cut_thread) {
        clock_t cpu_time = clock();
        usleep(250000);
        system_usage->cpu_usage = (((double) (clock() - cpu_time) / (double) CLOCKS_PER_SEC) / 0.25) * 100.0;
    }
    return NULL;
}
