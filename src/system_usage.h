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

#include <semaphore.h>
#include "ui.h"
#include "text.h"

typedef struct UISystemUsage_t {
    unsigned char * texture;
    Vec4            vertexes_attributes[4];
    Vec2            scale_matrix[2];
    Vec4            instance_offsets_buffer;
    GLuint          instance_offsets;
    GLuint          vertex_buffer_object;
    GLuint          vertex_array_object;
    GLchar          vertex_indexes[4];
    GLuint          texture_id;
    GLuint          index_buffer_object;
    UIText          ui_text_cpu;
    UIText          ui_text_mem;
    int             ready;
    int             update;
    sem_t           mutex;
    int             cut_thread;
} UISystemUsage;

typedef struct SystemUsage_t {
    double          cpu_usage;
    double          mem_usage;
    UISystemUsage * ui;
} SystemUsage;

void init_system_usage_ui(
    UISystemUsage * system_usage,
    const char * texture_system_usage_filename,
    const char * texture_text_filename,
    unsigned int texture_system_usage_width,
    unsigned int texture_system_usage_height,
    unsigned int texture_text_width,
    unsigned int texture_text_height,
    unsigned int viewport_width, 
    unsigned int viewport_height,
    GLfloat gl_x,
    GLfloat gl_y
);

void * update_system_usage(void * args);

#endif
