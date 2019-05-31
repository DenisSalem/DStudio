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
    vertexes_attributes[0].x = -1.0; \
    vertexes_attributes[0].y = 1.0; \
    vertexes_attributes[1].x = -1.0; \
    vertexes_attributes[1].y = -1.0; \
    vertexes_attributes[2].x =  1.0; \
    vertexes_attributes[2].y =  1.0; \
    vertexes_attributes[3].x =  1.0; \
    vertexes_attributes[3].y = -1.0;
     
#define DSTUDIO_SET_S_T_COORDINATES \
    vertexes_attributes[0].z = 0.0f; \
    vertexes_attributes[0].w = 0.0f; \
    vertexes_attributes[1].z = 0.0f; \
    vertexes_attributes[1].w = 1.0f; \
    vertexes_attributes[2].z = 1.0f; \
    vertexes_attributes[2].w = 0.0f; \
    vertexes_attributes[3].z = 1.0f; \
    vertexes_attributes[3].w = 1.0f;
    
#define DSTUDIO_SET_VERTEX_INDEXES \
    vertex_indexes[0] = 0; \
    vertex_indexes[1] = 1; \
    vertex_indexes[2] = 2; \
    vertex_indexes[3] = 3;
    
typedef struct Vec4_t {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} Vec4;

typedef struct vec2_t {
    GLfloat x;
    GLfloat y;
} Vec2;

void create_shader_program(GLuint * interactive_program_id, GLuint * non_interactive_program_id);
// png_bytep is basically unsigned char
int get_png_pixel(const char * filename, png_bytep * buffer);
void compile_shader(GLuint shader_id, GLchar ** source_pointer);
void load_shader(GLchar ** shader_buffer, const char * filename);

#endif
