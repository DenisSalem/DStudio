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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef DSTUDIO_DEBUG 
#include <string.h>
#endif

#include "../constants.h"
#include "../macros.h"
#include "../memory_management/dstudio_memory_management.h"
#include "../paths.h"

#include <GL/gl.h>
#include <GL/glx.h>

#include "extensions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifndef DSTUDIO_UI_ENGINE_H_INCLUDED
#define DSTUDIO_UI_ENGINE_H_INCLUDED

#define DSTUDIO_FLAG_USE_ALPHA                      1
#define DSTUDIO_FLAG_USE_ANTI_ALIASING              2
#define DSTUDIO_FLAG_TEXTURE_IS_PATTERN             4

typedef struct DStudioImage_t {
    uint_fast8_t * buffer;
    uint_fast32_t  width;
    uint_fast32_t  height;
    uint_fast8_t   channels;
} DStudioImage;

void dstudio_compile_shader(
    GLuint shader_id,
    GLchar ** source_pointer
);

GLuint dstudio_create_shader_program();

GLuint dstudio_create_gl_buffer(
    GLenum type,
    void * vertex_attributes,
    GLenum mode,
    uint_fast32_t data_size
);

DStudioImage dsudio_get_png_pixels(
    const char * filename
);

void dstudio_load_shader(
    GLchar ** shader_buffer,
    const char * filename
);

DStudioImage dsudio_read_png(const char * filename);

#endif
