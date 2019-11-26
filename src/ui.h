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
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

#include "window_management.h"

/*
 * This structure may be used to store opacity in
 * interactive_list usage case. For readability, the
 * third field is renamed.
 */
 
typedef struct Vec4_t {
    GLfloat x;
    GLfloat y;
    union {
        GLfloat z;
        GLfloat opacity;
    };
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

typedef struct UIElements_t {
    unsigned                    count;
    GLuint                      index_buffer_object;
    GLuint                      instance_offsets;
    GLfloat *                   instance_offsets_buffer; /* May be allocated either as array of Vec2 or Vec4 */
    GLuint                      instance_motions;
    GLfloat  *                  instance_motions_buffer;
    unsigned int                animated;
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

typedef struct UIElementSetting_t {
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
    /*
     * May be one of the following type
     * - UIElementSetting
     * - UIInteractiveListSetting
     */
    void *  settings;
    UIArea *            areas;
    UICallback *        callbacks;
    void (*update_callback) (int index, UIElements * context, void * args);
} UIElementSettingParams;

typedef union TextFieldContext_t {
    
} UITextFieldContext;

void compile_shader(
    GLuint shader_id,
    GLchar ** source_pointer
);

void configure_ui_element(
    UIElements * ui_elements,
    void * params
);

void create_shader_program(
    GLuint * interactive_program_id,
    GLuint * non_interactive_program_id
);

void gen_gl_buffer(
    GLenum type,
    GLuint * vertex_buffer_object_p,
    void * vertex_attributes,
    GLenum mode,
    unsigned int data_size
);

int get_png_pixel(
    const char * filename,
    png_bytep * buffer,
    png_uint_32 format // png_bytep is basically unsigned char
); 

void init_ui_element(
    GLfloat * instance_offset_p,
    float offset_x,
    float offset_y,
    GLfloat * motion_buffer
);

void init_ui_elements(
    int flags,
    UIElements * ui_elements,
    GLuint texture_id,
    unsigned int count,
    void (*configure_ui_element)(UIElements * ui_elements, void * params),
    void * params
);

void init_ui_elements_settings(
    UIElementSetting ** settings_p,
    GLfloat gl_x,
    GLfloat gl_y,
    GLfloat scale_area_x,
    GLfloat scale_area_y,
    GLfloat offset_x,
    GLfloat offset_y,
    unsigned int rows,
    unsigned int count,
    unsigned int ui_element_type
);

void load_shader(
    GLchar ** shader_buffer,
    const char * filename
);

void render_ui_elements(
    UIElements * ui_elements
);

// This one is not defined in the generic ui.c source file.
// Instead, it is defined for every tools from DStudio.
void render_viewport(unsigned int mask);

GLuint setup_texture_n_scale_matrix(
    unsigned int flags,
    GLuint texture_width,
    GLuint texture_height,
    const char * texture_filename,
    Vec2 * scale_matrix
);

void update_and_render(
    sem_t * mutex,
    unsigned int * update,
    void (*update_callback)(),
    GLuint scissor_x,
    GLuint scissor_y,
    GLuint scissor_width,
    GLuint scissor_height,
    unsigned int render_flag
);

void update_ui_element_motion(
    int index,
    UIElements * knobs_p,
    void * args
);

extern const unsigned int DSTUDIO_VIEWPORT_WIDTH;
extern const unsigned int DSTUDIO_VIEWPORT_HEIGHT;

#endif
