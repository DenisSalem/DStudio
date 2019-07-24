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

#ifndef SYSTEM_USAGE_INCLUDED
#define SYSTEM_USAGE_INCLUDED

#include "ui.h"

typedef struct UISystemUsage_t {
    unsigned char *             texture;
    Vec4                        vertexes_attributes[4];
    Vec2                        scale_matrix[2];
    GLuint                      vertex_buffer_object;
    GLuint                      vertex_array_object;
    GLuint                      vertex_indexes[4];
    GLuint                      texture_id;
    GLuint                      index_buffer_object;
} UISystemUsage;

typedef struct SystemUsage_t {
    double cpu_usage;
    int cut_thread;
    void (*update_ui)(double cpu_usage, double mem_usage);
    UISystemUsage * ui;
} SystemUsage;

void init_system_usage_ui(UISystemUsage * system_usage, const char * texture_filename, int texture_width, int texture_height, int viewport_width, int viewport_height);
void * update_system_usage(void * args);

#endif
