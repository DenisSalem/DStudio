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

#define DSTUDIO_SET_VERTEX_ATTRIBUTES \
    vertexes_attributes[0].x = -width / 2; \
    vertexes_attributes[0].y = height / 2; \
    vertexes_attributes[1].x = -width / 2; \
    vertexes_attributes[1].y = -height / 2; \
    vertexes_attributes[2].x =  width / 2; \
    vertexes_attributes[2].y =  height / 2; \
    vertexes_attributes[3].x =  width / 2; \
    vertexes_attributes[3].y = -height / 2;
     
#define DSTUDIO_SET_S_T_COORDINATES \
    vertexes_attributes[0].s = 0.0f; \
    vertexes_attributes[0].t = 0.0f; \
    vertexes_attributes[1].s = 0.0f; \
    vertexes_attributes[1].t = 1.0f; \
    vertexes_attributes[2].s = 1.0f; \
    vertexes_attributes[2].t = 0.0f; \
    vertexes_attributes[3].s = 1.0f; \
    vertexes_attributes[3].t = 1.0f;
    
#define DSTUDIO_SET_VERTEX_INDEXES \
    vertex_indexes[0] = 0; \
    vertex_indexes[1] = 1; \
    vertex_indexes[2] = 2; \
    vertex_indexes[3] = 3;
    
typedef struct Vec4_t {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
} Vec4;

typedef struct vec2_t {
    GLfloat x;
    GLfloat y;
} Vec2;

// png_bytep is basically unsigned char
int get_png_pixel(const char * filename, png_bytep * buffer);

void compile_shader(GLuint shader_id, GLchar ** source_pointer);
void load_shader(GLchar ** shader_buffer, const char * filename);

#endif
