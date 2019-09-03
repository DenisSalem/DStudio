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
#include "../knobs.h"
#include "../sliders.h"
#include "../system_usage.h"
#include "../instances.h"
#include "../text.h"
#include "../ui.h"

#define DSANDGRAINS_SAMPLE_KNOBS 8
#define DSANDGRAINS_SAMPLE_SMALL_KNOBS 10
#define DSANDGRAINS_VOICE_KNOBS 3

#define DSANDGRAINS_KNOBS_COUNT DSANDGRAINS_SAMPLE_KNOBS+DSANDGRAINS_SAMPLE_SMALL_KNOBS+DSANDGRAINS_VOICE_KNOBS
#define DSANDGRAINS_SLIDERS_COUNT_PER_GROUP 6
#define DSANDGRAINS_EQUALIZER_SLIDERS_COUNT 8
#define DSANDGRAINS_UI_ELEMENTS_COUNT (DSANDGRAINS_SLIDERS_COUNT_PER_GROUP * 4 ) + DSANDGRAINS_EQUALIZER_SLIDERS_COUNT + DSANDGRAINS_KNOBS_COUNT

#define DSANDGRAINS_BACKGROUND_ASSET_PATH   "../assets/dsandgrains_background.png"
#define DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH "../assets/system_usage.png"

#define DSANDGRAINS_BACKGROUND_TYPE_BACKGROUND      0
#define DSANDGRAINS_BACKGROUND_TYPE_SYSTEM_USAGE    1

typedef struct UIBackground_t {
    Vec4                vertexes_attributes[4];
    Vec2                scale_matrix[2];
    GLchar              vertex_indexes[4];
    unsigned char *     texture;
    GLuint              vertex_array_object;
    GLuint              index_buffer_object;
    GLuint              vertex_buffer_object;
    GLuint              texture_id;
    GLuint              instance_offsets;
    Vec4                instance_offsets_buffer;

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
    UISliders           sliders_dahdsr_pitch;
    UISliders           sliders_dahdsr_lfo;
    UISliders           sliders_dahdsr_lfo_pitch;
    UISliders           sliders_equalizer;
    UIKnobs             voice_knobs;
    UIArea              areas[DSANDGRAINS_UI_ELEMENTS_COUNT];
    UICallback          callbacks[DSANDGRAINS_UI_ELEMENTS_COUNT];
    UISystemUsage       system_usage;
    UIInstances         instances;
} UI;

static void init_background(UIBackground * background);

void * ui_thread(void * arg);
