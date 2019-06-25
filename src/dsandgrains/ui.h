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

#define DSANDGRAINS_SAMPLE_KNOBS 8
#define DSANDGRAINS_SAMPLE_SMALL_KNOBS 10
#define DSANDGRAINS_SLIDERS_DAHDSR 6
#define DSANDGRAINS_VOICE_KNOBS 3

#define DSANDGRAINS_KNOBS_COUNT DSANDGRAINS_SAMPLE_KNOBS+DSANDGRAINS_SAMPLE_SMALL_KNOBS+DSANDGRAINS_VOICE_KNOBS
#define DSANDGRAINS_SLIDERS_COUNT DSANDGRAINS_SLIDERS_DAHDSR
#define DSANDGRAINS_UI_ELEMENTS_COUNT DSANDGRAINS_SLIDERS_COUNT+DSANDGRAINS_KNOBS_COUNT

#define DSANDGRAINS_BACKGROUND_ASSET_PATH "../assets/dsandgrains_background.png"

#include "../knobs.h"
#include "../sliders.h"
#include "../ui.h"

typedef struct UIBackground_t {
    Vec4                vertexes_attributes[4];
    GLuint              vertex_indexes[6];
    unsigned char *     texture;
    GLuint              vertex_array_object;
    GLuint              index_buffer_object;
    GLuint              vertex_buffer_object;
    GLuint              texture_id;
} UIBackground;

typedef struct UICallback_t {
    void (*callback)(int index, void * context, void * args);
    int index;
    void * context_p;
    int type;
} UICallback;

typedef struct UI_t {
    UIBackground        background;
    UIKnobs             sample_knobs;
    UIKnobs             sample_small_knobs;
    UISliders           sliders_dahdsr;
    UIKnobs             voice_knobs;
    UIArea              areas[DSANDGRAINS_UI_ELEMENTS_COUNT];
    UICallback          callbacks[DSANDGRAINS_UI_ELEMENTS_COUNT];
} UI;

static void init_background(UIBackground * background);
static void render_background(UIBackground * background);

void * ui_thread(void * arg);
