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

#define DSTUDIO_KNOB_TYPE 1

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

inline float compute_knob_rotation(double xpos, double ypos, Vec2 active_knob_center) {
    float rotation = 0;

    float y = - ypos + active_knob_center.y;
    float x = xpos - active_knob_center.x;
    rotation = -atan(x / y);

    if (y < 0) {
        if (x < 0) {
            rotation += 3.141592;
        }
        else {
                rotation -= 3.141592;
        }
    }
    if (rotation > 2.356194) {
        rotation = 2.356194;
    }
    else if (rotation < -2.356194) {
        rotation = -2.356194;
    }
    return rotation;
}

void create_shader_program(GLuint * interactive_program_id, GLuint * non_interactive_program_id);
// png_bytep is basically unsigned char
int get_png_pixel(const char * filename, png_bytep * buffer, int alpha);
void compile_shader(GLuint shader_id, GLchar ** source_pointer);
void load_shader(GLchar ** shader_buffer, const char * filename);

#endif
