/*
 * Copyright 2019, 2025 Denis Salem
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef DSTUDIO_DEBUG 
#include <string.h>
#endif

#include "../constants.h"
#include "../macros.h"
#include "../memory_management/dstudio_memory_management.h"
#include "../paths.h"

#include <GL/gl.h>
#include <GL/glx.h>

#include "extensions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifndef DSTUDIO_UI_ENGINE_H_INCLUDED
#define DSTUDIO_UI_ENGINE_H_INCLUDED

typedef GLFWwindow* DStudioWindow ;
#define dstudio_window_should_close glfwWindowShouldClose

typedef struct DStudioGenericWidgetSharedObject_t {
    GLfloat vertices[16];
    GLuint  shader_program_id;
    GLuint  pos_location;
    GLuint  tex_location;
} DStudioGenericWidgetSharedObject;

typedef enum DStudioWidgetType_t {
    DSTUDIO_WIDGET_TYPE_KNOB,
    DSTUDIO_WIDGET_TYPE_SLIDER,
    DSTUDIO_WIDGET_TYPE_BUTTON
} DStudioWidgetType;

typedef struct DStudioImage_t {
    uint8_t * buffer;
    uint_fast32_t  width;
    uint_fast32_t  height;
    uint_fast8_t   channels;
    GLuint         texture_id;
} DStudioImage;

typedef struct Vec2_t {
    GLfloat x;
    GLfloat y;
} Vec2;

typedef struct Vec4_t {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} Vec4;

typedef struct DStudioBitmapWidget_t {
    DStudioWidgetType type;
    GLuint texture;
    GLuint background_texture;
    GLuint  vao_id;
    GLuint  vbo_id;
    uint_fast32_t  width;
    uint_fast32_t  height;
    uint_fast32_t  background_width;
    uint_fast32_t  background_height;
    Vec2 position;
    Vec2 offset;
    GLfloat rotation;
    Vec4 vertex_attributes;
    Vec2 widget_scale_matrix[2];
    Vec2 background_scale_matrix[2];
} DStudioBitmapWidget;

typedef struct DStudioSceneNode_t {
    Vec2 coordinates;
    Vec2 size;
    uint_fast32_t enabled;
    struct DStudioSceneNode_t * childs;
} DStudioSceneNode;

void dstudio_compile_shader(
    GLuint shader_id,
    GLchar ** source_pointer
);

GLuint dstudio_create_gl_buffer(
    GLenum type,
    void * vertex_attributes,
    GLenum mode,
    uint_fast32_t data_size
);

GLuint dstudio_create_shader_program();

DStudioImage dstudio_create_texture(
    const char * filename
);

DStudioBitmapWidget dstudio_create_widget(
    const char * widget_filename,
    const char * background_filename
);

DStudioImage dsudio_get_png_pixels(
    const char * filename
);

void dstudio_load_shader(
    GLchar ** shader_buffer,
    const char * filename
);

DStudioImage dsudio_read_png(const char * filename);

DStudioWindow dstudio_init_gui(int width, int height, const char * title);

// Will be removed to be hidden from API
void render_bitmap_widget(DStudioBitmapWidget widget);

extern uint32_t g_dstudio_viewport_width;
extern uint32_t g_dstudio_viewport_height;
extern DStudioGenericWidgetSharedObject generic_widget_shared_object;
#endif
