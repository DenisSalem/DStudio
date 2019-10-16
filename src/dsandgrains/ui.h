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

#define DSANDGRAINS_CPU_N_MEM_USAGE_X_POS 0.015
#define DSANDGRAINS_CPU_USAGE_Y_POS 0.916666
#define DSANDGRAINS_MEM_USAGE_Y_POS 0.862499
#define DSANDGRAINS_RESSOURCE_USAGE_PROMPT_X_POS -0.035
#define DSANDGRAINS_RESSOURCE_USAGE_PROMPT_Y_POS 0.889583
#define DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE 6
#define DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE 29

#define DSANDGRAINS_SAMPLE_KNOBS 8
#define DSANDGRAINS_SAMPLE_SMALL_KNOBS 10
#define DSANDGRAINS_VOICE_KNOBS 4
#define DSANDGRAINS_KNOBS_COUNT DSANDGRAINS_SAMPLE_KNOBS + DSANDGRAINS_SAMPLE_SMALL_KNOBS+DSANDGRAINS_VOICE_KNOBS

#define DSANDGRAINS_DAHDSR_SLIDERS_COUNT 6
#define DSANDGRAINS_EQUALIZER_SLIDERS_COUNT 8
#define DSANDGRAINS_INSTANCES_ARROWS 6

#define DSANDGRAINS_KNOB1_SCALE 64
#define DSANDGRAINS_KNOB2_SCALE 48
#define DSANDGRAINS_SLIDER1_SCALE 10
#define DSANDGRAINS_SYSTEM_USAGE_WIDTH 30
#define DSANDGRAINS_SYSTEM_USAGE_HEIGHT 23

#define DSANDGRAINS_BUTTONS_COUNT \
    DSANDGRAINS_INSTANCES_ARROWS + \
    DSANDGRAINS_SCROLLABLE_LIST_SIZE * 3

#define DSANDGRAINS_UI_ELEMENTS_COUNT \
    DSANDGRAINS_DAHDSR_SLIDERS_COUNT +\
    DSANDGRAINS_EQUALIZER_SLIDERS_COUNT +\
    DSANDGRAINS_KNOBS_COUNT +\
    DSANDGRAINS_BUTTONS_COUNT

#define DSANDGRAINS_KNOB1_ASSET_PATH        "../assets/knob1.png"
#define DSANDGRAINS_KNOB2_ASSET_PATH        "../assets/knob2.png"
#define DSANDGRAINS_SLIDER1_ASSET_PATH      "../assets/slider1.png"
#define DSANDGRAINS_BACKGROUND_ASSET_PATH   "../assets/dsandgrains_background.png"
#define DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH "../assets/system_usage.png"
#define DSANDGRAINS_SCROLLABLE_LIST_SIZE 7

void * ui_thread(void * arg);
