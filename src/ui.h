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

typedef struct vec2_t {
    GLfloat x;
    GLfloat y;
} Vec2;

/*
 * This structure may be used to store offsets or areas.
 */
 
typedef struct Vec4_t {
    union {
        GLfloat min_area_x;
        GLfloat top_left_x;
        GLfloat x;
    };
    union {
        GLfloat max_area_x;
        GLfloat top_left_y;
        GLfloat y;

    };
    union {
        GLfloat min_area_y;
        GLfloat bottom_right_x;
        GLfloat z;
    };
    union {
        GLfloat max_area_y;
        GLfloat bottom_right_y;
        GLfloat w;
    };
} Vec4;

typedef struct Scissor_t {
    GLint x;
  	GLint y;
  	GLsizei width;
  	GLsizei height;
} Scissor;

typedef enum UIElementType_t {
    DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND = 1,
    DSTUDIO_UI_ELEMENT_TYPE_TEXT = 2,
    DSTUDIO_UI_ELEMENT_TYPE_SLIDERS = 4,
    DSTUDIO_UI_ELEMENT_TYPE_KNOBS = 8
} UIElementType;

/*
 * UIElements describe any rendered elements. It holds both application
 * logic information and OpenGL buffers.
 * 
 * - count:             specify the number of instance. Should be set
 *                      to 1 by default except for text.
 * - render:            is a boolean allowing ui_element to be rendered
 *                      if set to 1.
 * - enabled:           is a boolean allowing element to be interactive.
 * - texture_index:     Point to the current texture id in textures_ids.
 * - groupe_identifier: Some elements are part of meta element, like
 *                      list or sliders group.
 * - texture_ids:       Array of texture id. Allow switching texture for
 *                      rebounce buttons or disabled ui elements like
 *                      knobs or sliders.   
 * - type:              specify the kind of ui element.
 * - vertex_attributes: Holds initial position of each vertex of the 
 *                      element as well as its initial UV coordinates.
 */
 
typedef struct UIElements_t UIElements;

typedef struct UIElements_t {
    unsigned int                count;
    unsigned int                render;
    unsigned int                enabled;
    unsigned int                texture_index;
    unsigned int                group_identifier;
    GLchar                      vertex_indexes[4];
    GLuint                      texture_ids[2];
    GLuint                      vertex_array_object;
    GLuint                      vertex_buffer_object;
    GLuint                      index_buffer_object;
    GLuint                      instance_alphas;
    GLuint                      instance_motions;
    GLuint                      motion_type;
    GLuint                      instance_offsets;
    GLfloat *                   instance_alphas_buffer; 
    GLfloat *                   instance_motions_buffer;
    Vec4 *                      instance_offsets_buffer; 
    Vec4                        vertex_attributes[4];
    Vec4                        areas;
    Scissor                     scissor;
    Vec2 *                      scale_matrix;
    UIElementType               type;
    void (*application_callback)(UIElements * self, void * args);
} UIElements;

void compile_shader(
    GLuint shader_id,
    GLchar ** source_pointer
);

void create_shader_program(
    GLuint * shader_program_id
);

void init_ui_elements(
    int flags,
    UIElements * ui_elements
);

void init_ui_elements_array(
    UIElements * ui_elements_array,
    GLuint * texture_ids,
    Vec2 * scale_matrix,
    GLfloat gl_x,
    GLfloat gl_y,
    GLfloat scale_area_x,
    GLfloat scale_area_y,
    GLfloat offset_x,
    GLfloat offset_y,
    unsigned int columns,
    unsigned int count,
    unsigned int instances_count,
    UIElementType ui_element_type
);

int get_png_pixel(
    const char * filename,
    png_bytep * buffer,
    png_uint_32 format // png_bytep is basically unsigned char
); 

void load_shader(
    GLchar ** shader_buffer,
    const char * filename
);

void manage_cursor_position(
    int xpos,
    int ypos
);

void manage_mouse_button(
    int xpos,
    int ypos,
    int button,
    int action
);

GLuint setup_texture_n_scale_matrix(
    unsigned int flags,
    GLuint texture_width,
    GLuint texture_height,
    const char * texture_filename,
    Vec2 * scale_matrix
);

// Must be defined by consumer
typedef struct UIElementsArray_t UIElementsArray;
extern UIElementsArray g_ui_elements_array;

extern GLint scissor_x, scissor_y;
extern GLsizei scissor_width, scissor_height;
extern const unsigned int g_dstudio_viewport_width;
extern const unsigned int g_dstudio_viewport_height;

/*
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
    
     //May be one of the following type
     //- UIElementSetting
     //- UIInteractiveListSetting
     //
    void *  settings;
    UIArea *            areas;
    UICallback *        callbacks;
    void (*update_callback) (int index, UIElements * context, void * args);
} UIElementSettingParams;

typedef union TextFieldContext_t {
    
} UITextFieldContext;

void configure_ui_element(
    UIElements * ui_elements,
    void * params
);

void gen_gl_buffer(
    GLenum type,
    GLuint * vertex_buffer_object_p,
    void * vertex_attributes,
    GLenum mode,
    unsigned int data_size
);

void init_ui_element(
    GLfloat * instance_offset_p,
    float offset_x,
    float offset_y,
    GLfloat * motion_buffer
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
*/

void render_ui_elements(
    UIElements * ui_elements
);

/*
// This one is not defined in the generic ui.c source file.
// Instead, it is defined for every tools from DStudio.
void render_viewport(unsigned int mask);

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
*/

#define DEFINE_RENDER_VIEWPORT \
    void render_viewport(unsigned int render_all) { \
        unsigned int ui_elements_count = sizeof(UIElementsArray) / sizeof(UIElements); \
        UIElements * ui_elements_array = (UIElements *) &g_ui_elements_array; \
        if (g_ui_elements_array.background.render || render_all) { \
            glScissor( \
                g_ui_elements_array.background.scissor.x, \
                g_ui_elements_array.background.scissor.y, \
                g_ui_elements_array.background.scissor.width, \
                g_ui_elements_array.background.scissor.height \
            ); \
            glUniformMatrix2fv( \
                g_scale_matrix_id, \
                1, \
                GL_FALSE, \
                (float *) g_ui_elements_array.background.scale_matrix \
            ); \
            render_ui_elements(&g_ui_elements_array.background); \
            g_ui_elements_array.background.render = 0; \
        } \
        else { \
            for (unsigned int i = 1; i < ui_elements_count; i++) { \
                if (ui_elements_array[i].render) { \
                    glScissor( \
                        ui_elements_array[i].scissor.x, \
                        ui_elements_array[i].scissor.y, \
                        ui_elements_array[i].scissor.width, \
                        ui_elements_array[i].scissor.height \
                    ); \
                    glUniformMatrix2fv( \
                        g_scale_matrix_id, \
                        1, \
                        GL_FALSE, \
                        (float *) g_ui_elements_array.background.scale_matrix \
                    ); \
                    render_ui_elements(&g_ui_elements_array.background); \
                }\
            } \
        } \
        for (unsigned int i = 1; i < ui_elements_count; i++) { \
            if (ui_elements_array[i].render || render_all) { \
                glScissor( \
                    ui_elements_array[i].scissor.x, \
                    ui_elements_array[i].scissor.y, \
                    ui_elements_array[i].scissor.width, \
                    ui_elements_array[i].scissor.height \
                ); \
                glUniformMatrix2fv( \
                    g_scale_matrix_id, \
                    1, \
                    GL_FALSE, \
                    (float *) ui_elements_array[i].scale_matrix \
                ); \
                render_ui_elements(&ui_elements_array[i]); \
                ui_elements_array[i].render = 0; \
            }\
        } \
    }
#endif
