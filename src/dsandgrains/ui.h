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

#define DSANDGRAINS_VIEWPORT_WIDTH  800
#define DSANDGRAINS_VIEWPORT_HEIGHT 480

#include "../knobs.h"
#include "../ui.h"

typedef struct UiBackground_t {
    Vec4    vertexes_attributes[4];
    GLuint              vertex_indexes[6];
    unsigned char *     texture;
    GLuint              vertex_array_object;
    GLuint              index_buffer_object;
    GLuint              vertex_buffer_object;
    GLuint              texture_id;
} UiBackground;

typedef struct UI_t {
    UiBackground  background;
    UiKnobs       sample_knobs;
} UI;

static void free_background(UiBackground * background);
static void init_background(UiBackground * background);
static void render_background(UiBackground * background);
void * ui_thread(void * arg);
