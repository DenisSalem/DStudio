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

typedef enum DStudioWidgetType_t {
    DSTUDIO_WIDGET_TYPE_NONE,
    DSTUDIO_WIDGET_TYPE_KNOB,
    DSTUDIO_WIDGET_TYPE_SLIDER,
    DSTUDIO_WIDGET_TYPE_BUTTON
} DStudioWidgetType;

typedef enum DStudioSceneNodeType_t {
    DSTUDIO_SCENE_NODE_TYPE_ROOT,
    DSTUDIO_SCENE_NODE_TYPE_STACK,
    DSTUDIO_SCENE_NODE_TYPE_BITMAP_WIDGET,
} DStudioSceneNodeType;

typedef enum DStudioSceneNodeBitFlag_t {
    DSTUDIO_SCENE_NODE_ENABLED = 1,
    DSTUDIO_SCENE_NODE_STACK_VERTICAL = 2,
} DStudioSceneNodeBitFlag;

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

typedef struct DStudioGenericWidgetSharedObject_t {
    Vec4 base_vertices[4];
    Vec4 vertices[4];
    GLuint  shader_program_id;
    GLuint  pos_location;
    GLuint  tex_location;
} DStudioGenericWidgetSharedObject;

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
    GLfloat rotation;
} DStudioBitmapWidget;

typedef struct DStudioSceneNode_t {
    uint_fast8_t type;
    uint_fast8_t flag;
    uint_fast32_t childs_count;
    struct DStudioSceneNode_t * childs;
    Vec2 coordinates;
    Vec2 size;
    union {
        DStudioBitmapWidget bitmap; 
    } widget;
} DStudioSceneNode;

DStudioSceneNode * dstudio_create_scene_tree();

DStudioBitmapWidget dstudio_create_knob(
    const char * widget_filename,
    const char * background_filename
);

void dstudio_event_manager();

void dstudio_init_gui(int width, int height, const char * title);

void dstudio_render_scene_tree();

int dstudio_window_should_close();

#endif
