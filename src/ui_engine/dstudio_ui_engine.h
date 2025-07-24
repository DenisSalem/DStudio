/*
 * Copyright 2019, 2025 Denis Salem
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

#ifndef DSTUDIO_UI_ENGINE_H_INCLUDED
#define DSTUDIO_UI_ENGINE_H_INCLUDED

#include "../memory_management/dstudio_memory_management.h"

#include <GL/gl.h>
#include <GL/glx.h>

#include "extensions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void dstudio_compile_shader(
    GLuint shader_id,
    GLchar ** source_pointer
);

void dstudio_create_shader_program(
    GLuint * shader_program_id
);

void dstudio_gen_gl_buffer(
    GLenum type,
    GLuint * vertex_buffer_object_p,
    void * vertex_attributes,
    GLenum mode,
    uint_fast32_t data_size
);

void dstudio_load_shader(
    GLchar ** shader_buffer,
    const char * filename
);

#endif
