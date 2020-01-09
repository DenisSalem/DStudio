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

#include "common.h"
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
    DSTUDIO_UI_ELEMENT_TYPE_SLIDER = 4,
    DSTUDIO_UI_ELEMENT_TYPE_KNOB = 8
} UIElementType;

typedef enum MotionType_t {
    DSTUDIO_MOTION_TYPE_NONE = 0U,
    DSTUDIO_MOTION_TYPE_ROTATION = 1U,
    DSTUDIO_MOTION_TYPE_SLIDE = 2U
} MotionType;

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

void init_opengl_ui_elements(
    int flags,
    UIElements * ui_elements
);

void init_ui_elements(
    UIElements * ui_elements_array,
    GLuint * texture_ids,
    Vec2 * scale_matrix,
    GLfloat gl_x,
    GLfloat gl_y,
    GLfloat area_width,
    GLfloat area_height,
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
typedef struct UIElementsStruct_t UIElementsStruct;
extern UIElementsStruct g_ui_elements_struct;
extern UIElements * g_ui_elements_array;

extern GLint scissor_x, scissor_y;
extern GLsizei scissor_width, scissor_height;
extern const unsigned int g_dstudio_ui_element_count;
extern const unsigned int g_dstudio_viewport_width;
extern const unsigned int g_dstudio_viewport_height;
extern GLuint g_shader_program_id;
extern GLuint g_scale_matrix_id;
extern GLuint g_motion_type_location;

void gen_gl_buffer(
    GLenum type,
    GLuint * vertex_buffer_object_p,
    void * vertex_attributes,
    GLenum mode,
    unsigned int data_size
);

void render_ui_elements(
    UIElements * ui_elements
);

void render_viewport(unsigned int render_all);

void update_threaded_ui_element(
    ThreadControl * thread_control,
    void (*update_callback)()
);

void update_ui_element_motion(
    UIElements * ui_elements_p,
    float motion
);

#endif
