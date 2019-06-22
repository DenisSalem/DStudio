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

#define DSTUDIO_FRAGMENT_SHADER_PATH "../assets/fragment.shader"
#define DSTUDIO_INTERACTIVE_VERTEX_SHADER_PATH "../assets/interactive_vertex.shader"
#define DSTUDIO_NON_INTERACTIVE_VERTEX_SHADER_PATH "../assets/non_interactive_vertex.shader"
#define DSANDGRAINS_KNOB1_ASSET_PATH "../assets/knob1.png"
#define DSANDGRAINS_KNOB2_ASSET_PATH "../assets/knob2.png"
#define DSANDGRAINS_SLIDER1_ASSET_PATH "../assets/slider1.png"

#ifdef DSTUDIO_USE_GLFW3
    #include <GLFW/glfw3.h>
#endif

#include <math.h>
#include <png.h>
#include <string.h>
#include <stdlib.h>

#define DSTUDIO_SET_AREA(index, mn_x, mx_x, mn_y, mx_y) \
    ui_areas[index].min_x = mn_x; \
    ui_areas[index].max_x = mx_x; \
    ui_areas[index].min_y = mn_y; \
    ui_areas[index].max_y = mx_y; \
    ui_areas[index].x     = (mn_x + mx_x ) / 2;\
    ui_areas[index].y     = (mn_y + mx_y ) / 2;

#define DSTUDIO_SET_UI_CALLBACK(array_index, input_callback, input_index, input_context, input_type) \
    ui_callbacks[array_index].callback = input_callback; \
    ui_callbacks[array_index].index = input_index; \
    ui_callbacks[array_index].context_p = input_context; \
    ui_callbacks[array_index].type = input_type;

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

#define DSTUDIO_KNOB_TYPE_1 1
#define DSTUDIO_KNOB_TYPE_2 2

typedef struct UIArea_t {
    float min_x;
    float min_y;
    float max_x;
    float max_y;
    float x;
    float y;
} UIArea;

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

void compile_shader(GLuint shader_id, GLchar ** source_pointer);

void create_shader_program(GLuint * interactive_program_id, GLuint * non_interactive_program_id);

void finalize_ui_element( int count, GLuint * instance_offsets_p, Vec2 * instance_offsets_buffer, GLuint * instance_motions_p, GLfloat * instance_motions_buffer, GLuint * vertex_array_object_p, GLuint vertex_buffer_object);
int get_png_pixel(const char * filename, png_bytep * buffer, png_uint_32 format); // png_bytep is basically unsigned char
void init_ui_element(Vec2 * instance_offset_p, float offset_x, float offset_y, GLfloat * motion_buffer);
void init_ui_elements_cpu_side(int count, int * count_p, GLuint texture_scale, GLuint * texture_scale_p, const char * texture_filename, unsigned char ** texture_p, Vec2 ** offsets_buffer_p, GLfloat ** motions_buffer_p, GLchar * vertex_indexes, Vec2 * scale_matrix, int viewport_width, int viewport_height);
void init_ui_elements_gpu_side(int enable_aa, Vec4 * vertexes_attributes, GLuint * vertex_buffer_object_p, GLuint * texture_id_p, GLuint texture_scale, unsigned char * texture, GLuint * index_buffer_object_p, GLchar * vertex_indexes);
void load_shader(GLchar ** shader_buffer, const char * filename);
void render_ui_elements(GLuint texture_id, GLuint vertex_array_object, GLuint index_buffer_object, int count);

#endif
