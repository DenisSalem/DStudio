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

#include <math.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "window_management.h"

#define DSTUDIO_FRAGMENT_SHADER_PATH                "../assets/fragment.shader"
#define DSTUDIO_INTERACTIVE_VERTEX_SHADER_PATH      "../assets/interactive_vertex.shader"
#define DSTUDIO_NON_INTERACTIVE_VERTEX_SHADER_PATH  "../assets/non_interactive_vertex.shader"
#define DSANDGRAINS_KNOB1_ASSET_PATH                "../assets/knob1.png"
#define DSANDGRAINS_KNOB2_ASSET_PATH                "../assets/knob2.png"
#define DSANDGRAINS_SLIDER1_ASSET_PATH              "../assets/slider1.png"
#define DSTUDIO_CHAR_TABLE_ASSET_PATH               "../assets/char_table.png"
#define DSTUDIO_CHAR_TABLE_SMALL_ASSET_PATH         "../assets/char_table_small.png"
#define DSTUDIO_CHAR_TABLE_ASSET_WIDTH              104
#define DSTUDIO_CHAR_TABLE_ASSET_HEIGHT             234
#define DSTUDIO_RENDER_ALL              0xfffffff
#define DSTUDIO_RENDER_KNOBS            1
#define DSTUDIO_RENDER_SLIDERS          2
#define DSTUDIO_RENDER_SYSTEM_USAGE     4
#define DSTUDIO_RENDER_INSTANCES        8

extern const unsigned int DSTUDIO_VIEWPORT_WIDTH;
extern const unsigned int DSTUDIO_VIEWPORT_HEIGHT;

#define DSTUDIO_SET_TEXT_SCALE_MATRIX(matrix, width, height) \
    matrix[0].x = ((float) width / (float) DSTUDIO_VIEWPORT_WIDTH) / DSTUDIO_CHAR_SIZE_DIVISOR; \
    matrix[0].y = 0; \
    matrix[1].x = 0; \
    matrix[1].y = ((float) height / (float) DSTUDIO_VIEWPORT_HEIGHT) / DSTUDIO_CHAR_SIZE_DIVISOR;

#define DSTUDIO_KNOB_TYPE_1 1
#define DSTUDIO_KNOB_TYPE_2 2
#define DSTUDIO_SLIDER_TYPE_1 4

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

typedef struct UITexture_t {
    GLuint id;
    GLuint width;
    GLuint height;
} UITexture;

typedef struct UIElement_t {
    unsigned                    count;
    GLuint                      index_buffer_object;
    GLuint                      instance_offsets;
    GLfloat *                   instance_offsets_buffer; /* May be allocated either as array of Vec2 or Vec4 */
    GLuint                      instance_motions;
    GLfloat  *                  instance_motions_buffer;
    unsigned int                interactive;
    Vec2                        scale_matrix[2];
    unsigned char *             texture;
    GLuint                      texture_id;
    GLuint                      vertex_array_object;
    Vec4                        vertex_attributes[4];  
    GLuint                      vertex_buffer_object;
    GLchar                      vertex_indexes[4];
} UIElements;

typedef struct UICallback_t {
    void (*callback)(int index, UIElements * context, void * args);
    int index;
    void * context_p;
    int type;
} UICallback;

typedef struct UIArea_t {
    float min_x;
    float min_y;
    float max_x;
    float max_y;
    float x;
    float y;
} UIArea;

/* Obsolete */
typedef struct InitUIElementArray_t {
    GLfloat gl_x;
    GLfloat gl_y;
    GLfloat min_area_x;
    GLfloat max_area_x;
    GLfloat min_area_y;
    GLfloat max_area_y;
    unsigned char ui_element_type;
} InitUIElementArray;

typedef struct ui_element_setting_t {
    GLfloat gl_x;
    GLfloat gl_y;
    GLfloat min_area_x;
    GLfloat max_area_x;
    GLfloat min_area_y;
    GLfloat max_area_y;
    unsigned char ui_element_type;
} UIElementSetting;

typedef struct ui_element_setting_params_t {
    unsigned int        array_offset;
    UIElementSetting *  settings;
    UIArea *            areas;
    UICallback *        callbacks;
    void (*update_callback) (int index, UIElements * context, void * args);
} UIElementSettingParams;

void compile_shader(GLuint shader_id, GLchar ** source_pointer);
void configure_ui_element(UIElements * ui_elements, void * params);
void create_shader_program(GLuint * interactive_program_id, GLuint * non_interactive_program_id);
void gen_gl_buffer(GLenum type, GLuint * vertex_buffer_object_p,  void * vertex_attributes, GLenum mode, unsigned int data_size);
int get_png_pixel(const char * filename, png_bytep * buffer, png_uint_32 format); // png_bytep is basically unsigned char
void init_ui_elements(UIElements * ui_elements, GLuint texture_id, unsigned int count, void (*configure_ui_element)(UIElements * ui_elements, void * params), void * params);
void init_ui_element(GLfloat * instance_offset_p, float offset_x, float offset_y, GLfloat * motion_buffer);
void load_shader(GLchar ** shader_buffer, const char * filename);
void render_ui_elements(UIElements * ui_elements);
GLuint setup_texture_n_scale_matrix(int enable_aa, int alpha, GLuint texture_width, GLuint texture_height, const char * texture_filename, Vec2 * scale_matrix);
 
#endif
