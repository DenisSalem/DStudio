/*
 * Copyright 2019, 2021 Denis Salem
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
#include "interactive_list.h"

#include "transition_animation.h"

#define SET_UI_MENU_BACKGROUND_INDEX \
    for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) { \
        if (&g_ui_elements_array[i] == &g_ui_elements_struct.menu_background) { \
            g_menu_background_index = i; \
            break; \
        }\
    }

typedef struct PatternScale_t {
    unsigned int width;
    unsigned int height;
} PatternScale;

typedef struct vec2_t {
    GLfloat x;
    GLfloat y;
} Vec2;

/*
 * This structure may be used to store offsets or areas.
 */
 
typedef struct Vec4_t {
    union {
        GLfloat r;
        GLfloat min_area_x;
        GLfloat top_left_x;
        GLfloat x;
    };
    union {
        GLfloat g;
        GLfloat max_area_x;
        GLfloat top_left_y;
        GLfloat y;
    };
    union {
        GLfloat b;
        GLfloat min_area_y;
        GLfloat bottom_right_x;
        GLfloat z;
    };
    union {
        GLfloat a;
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

typedef struct UpdaterRegister_t {
    void (*updater)();
} UpdaterRegister;

typedef enum UIElementType_t {
    DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND = 1,
    DSTUDIO_UI_ELEMENT_TYPE_BUTTON = 2,
    DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE = 4,
    DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM = 8,
    DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT = 16,
    DSTUDIO_UI_ELEMENT_TYPE_KNOB = 32,
    DSTUDIO_UI_ELEMENT_TYPE_LAYER = 64,
    DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM = 128,
    DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE = 256,
    DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND = 512,
    DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BAR_PLOT = 1024,
    DSTUDIO_UI_ELEMENT_TYPE_PATTERN = 2048,
    DSTUDIO_UI_ELEMENT_TYPE_PATTERN_BACKGROUND = 4096,
    DSTUDIO_UI_ELEMENT_TYPE_SLIDER = 8192,
    DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND = 16384,
    DSTUDIO_UI_ELEMENT_TYPE_TEXT = 32768,
    DSTUDIO_UI_ELEMENT_TYPE_TEXT_BACKGROUND = 65536
} UIElementType;

typedef enum UIElementRenderState_t {
    DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED = 0,
    DSTUDIO_UI_ELEMENT_RENDER_REQUESTED = 1,
    DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED = 2,
} UIElementRenderState;

#define  DSTUDIO_ANY_TEXT_TYPE \
    (DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM | \
    DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM | \
    DSTUDIO_UI_ELEMENT_TYPE_TEXT | \
    DSTUDIO_UI_ELEMENT_TYPE_TEXT_BACKGROUND)
    
#define DSTUDIO_ANY_PATTERN_TYPE \
    (DSTUDIO_UI_ELEMENT_TYPE_PATTERN | \
    DSTUDIO_UI_ELEMENT_TYPE_PATTERN_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT)
    
#define DSTUDIO_ANY_BACKGROUND_TYPE \
    (DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_PATTERN_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_TEXT_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_LAYER)

#define DSTUDIO_ANY_NO_TEXTURE_TYPE \
    (DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE | \
    DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND | \
    DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BAR_PLOT)

// Used internally only
typedef struct Area_t {
    GLfloat min_x;
    GLfloat min_y;
    GLfloat max_x;
    GLfloat max_y;
    GLfloat offset_x;
    GLfloat offset_y;
} Area;

typedef enum MotionType_t {
    DSTUDIO_MOTION_TYPE_NONE = 0U,
    DSTUDIO_MOTION_TYPE_ROTATION = 1U,
    DSTUDIO_MOTION_TYPE_SLIDE = 2U,
    DSTUDIO_MOTION_TYPE_BAR_PLOT = 3U
} MotionType;

typedef struct UIElementsCoordinatesSettings_t {
    Vec2 *                      scale_matrix;
    Vec4 *                      instance_offsets_buffer;
    Scissor                     scissor;
    Scissor                     previous_scissor;
} UIElementsCoordinatesSettings;

/*
 * UIElements describe any rendered elements. It holds both application
 * logic information and OpenGL buffers.
 * 
 * - count:             specify the number of instance. Should be set
 *                      to 1 by default except for text.
 * - render_state:      Store value from UIElementRenderState enumeration
 *                      and tell engine what to do.
 * - enabled:           is a boolean allowing element to be interactive.
 * - texture_index:     Point to the current texture id in textures_ids.
 * - interactive_list:  Some elements are part of meta element, like
 *                      list or sliders group.
 * - texture_ids:       Array of texture id. Allow switching texture for
 *                      rebounce buttons or disabled ui elements like
 *                      knobs or sliders.   
 * - type:              specify the kind of ui element.
 * - vertex_attributes: Holds initial position of each vertex of the 
 *                      element as well as its initial UV coordinates.
 * - timestamp:         Used only for buttons to animate rebounce.
 */
 
typedef struct UIElements_t UIElements;
typedef struct UIInteractiveList_t UIInteractiveList;
typedef struct TransitionAnimation_t TransitionAnimation;

typedef struct UIElements_t {
    unsigned int                count;
    unsigned int                flags;
    unsigned char               render_state;
    unsigned char               visible;
    unsigned char               enabled;
    unsigned char               texture_index;
    double                      timestamp;
    union {
        unsigned int            text_buffer_size;
        GLfloat                 previous_slider_motion;
    };
    GLchar                      vertex_indexes[4];
    /* For any text type there is only one index used for texture because
     * it's not meant to be changed. The other index is used to store the
     * previous string size necessary to compute optimized text area.
     * In this configuration, the first index is the previous text size,
     * and the second one refer to the texture identifier. The previous 
     * implies that for textual type texture index is always set to 1.*/
    union {
        GLuint                  texture_ids[2];
        GLuint                  previous_text_size;
        /* Similarly, highlight need to be rendered twice at two location
         different location. The previous one is stored there.*/
    };
    GLuint                      vertex_array_object;
    GLuint                      vertex_buffer_object;
    GLuint                      index_buffer_object;
    GLuint                      instance_alphas;
    GLuint                      instance_motions;
    GLuint                      instance_offsets;
    GLfloat *                   instance_alphas_buffer; 
    GLfloat *                   instance_motions_buffer;
    Vec4                        vertex_attributes[4];
    UIElementsCoordinatesSettings coordinates_settings;
    Vec4                        areas;
    union {
        PatternScale            pattern_scale;        
        Vec4                    color;
    };
    UIElements *                overlap_sub_menu_ui_elements;
    UIElementType               type;
    UIInteractiveList *         interactive_list;
    void *                      application_callback_args;
    void (*application_callback)(UIElements * self);
    TransitionAnimation *       transition_animation;
} UIElements;

void compile_shader(
    GLuint shader_id,
    GLchar ** source_pointer
);

void create_shader_program(
    GLuint * shader_program_id
);

void init_extended_background(char * texture_path, unsigned int width, unsigned int height);

void init_opengl_ui_elements(
    int flags,
    UIElements * ui_elements
);

void init_text();

void init_ui_element_updater_register(
    unsigned int updater_count
);

void init_ui();

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
    UIElementType ui_element_type,
    int flags
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

void render_ui_elements(
    UIElements * ui_elements
);

void register_ui_elements_updater(
    void (*updater)()
);

void render_loop();

/* Non zero value returned means that swap buffering is required */
unsigned int render_viewport(unsigned int render_all);

void set_prime_interface(
    unsigned int state
);

void set_ui_elements_visibility(
    UIElements * ui_elements,
    unsigned int state,
    unsigned int count
);

GLuint setup_texture_n_scale_matrix(
    unsigned int flags,
    GLuint texture_width,
    GLuint texture_height,
    const char * texture_filename,
    Vec2 * scale_matrix,
    PatternScale * pattern_scale
);

void update_gpu_buffer(
    UIElements * ui_element
);

void update_ui_elements();

void update_ui_element_motion(
    UIElements * ui_elements_p,
    float motion
);

// Used when window is forced to be resized
void update_viewport(WindowScale window_scale);

// Must be defined by consumer
typedef struct UIElementsStruct_t UIElementsStruct;
extern UIElementsStruct g_ui_elements_struct;
extern UIElements * g_ui_elements_array;

extern int          g_active_slider_range_max;
extern int          g_active_slider_range_min;
extern Vec2         g_background_scale_matrix[2];
extern GLuint       g_charset_8x18_texture_ids[2];
extern GLuint       g_charset_4x9_texture_ids[2];
extern Vec2         g_charset_8x18_scale_matrix[2];
extern Vec2         g_charset_4x9_scale_matrix[2];
extern const unsigned int g_dstudio_ui_element_count;
extern const unsigned int g_dstudio_viewport_width;
extern const unsigned int g_dstudio_viewport_height;
extern GLint        scissor_x, scissor_y;
extern GLsizei      scissor_width, scissor_height;
extern GLint        g_saved_scissor_y;
extern GLuint       g_shader_program_id;
extern GLuint       g_scale_matrix_id;
extern GLuint       g_motion_type_location;
extern GLuint       g_no_texture_location;
extern GLuint       g_ui_element_color_location;
extern int          g_ui_element_center_x;
extern int          g_ui_element_center_y;
extern unsigned int g_framerate;
extern unsigned int g_menu_background_index;
extern UIElements * g_menu_background_enabled;
extern unsigned int g_request_render_all;
extern WindowScale  g_previous_window_scale;

#endif
