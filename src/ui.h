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

#ifndef DSTUDIO_UI_H_INCLUDED
#define DSTUDIO_UI_H_INCLUDED

#ifdef DSTUDIO_USE_GLFW3
    #include <GLFW/glfw3.h>
#endif

#include <png.h>
#include <string.h>
#include <stdlib.h>

typedef struct VertexAttributes_t {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} VertexAttributes;

/* 
 * png_bytep is basically unsigned char
 */
int get_png_pixel(const char * filename, png_bytep * buffer);

void compile_shader(GLuint shader_id, GLchar ** source_pointer);
void load_shader(GLchar ** shader_buffer, const char * filename);

#endif
