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

#include "../system_usage.h"
#include "../instances.h"
#include "../text.h"
#include "../ui.h"

// TODO: SORT CONSTANT BY USAGE, THEN BY NAME

#define APPLICATION_NAME "D S A N D G R A I N S"
#define DSANDGRAINS_CPU_N_MEM_USAGE_X_POS 0.015
#define DSANDGRAINS_CPU_USAGE_Y_POS 0.916666
#define DSANDGRAINS_EQUALIZER_POS_X 0.32
#define DSANDGRAINS_EQUALIZER_POS_Y -0.425
#define DSANDGRAINS_DAHDDSR_POS_X 0.0275
#define DSANDGRAINS_DAHDDSR_POS_Y -0.470833
#define DSANDGRAINS_VOICE_KNOBS_POS_X 0.3725
#define DSANDGRAINS_VOICE_KNOBS_POS_Y 0.2708
#define DSANDGRAINS_VOICE_KNOBS_OFFSET_X 0.175
#define DSANDGRAINS_VOICE_KNOBS_OFFSET_Y -0.3416
#define DSANDGRAINS_VOICE_KNOBS_ROWS 2
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_X 0.0375
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_Y 0.3
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_X 0.13
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_Y 0
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_ROWS 2
#define DSANDGRAINS_LFO_KNOBS_POS_X -0.2225
#define DSANDGRAINS_LFO_KNOBS_POS_Y 0.25
#define DSANDGRAINS_LFO_KNOBS_OFFSET_X 0.12
#define DSANDGRAINS_LFO_KNOBS_OFFSET_Y -0.2483
#define DSANDGRAINS_LFO_KNOBS_ROWS 2
#define DSANDGRAINS_LFO_PITCH_KNOBS_POS_X -0.2225
#define DSANDGRAINS_LFO_PITCH_KNOBS_POS_Y -0.5125
#define DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_X 0.12
#define DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_Y -0.2483
#define DSANDGRAINS_LFO_PITCH_KNOBS_ROWS 2

#define DSANDGRAINS_SAMPLE_KNOBS_POS_X -0.8675
#define DSANDGRAINS_SAMPLE_KNOBS_POS_Y 0.25
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X 0.16
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y -0.4
#define DSANDGRAINS_SAMPLE_KNOBS_ROWS 4

#define DSANDGRAINS_TINY_BUTTON_SCALE 32
#define DSANDGRAINS_MEM_USAGE_Y_POS 0.862499
#define DSANDGRAINS_RESSOURCE_USAGE_PROMPT_X_POS -0.035
#define DSANDGRAINS_RESSOURCE_USAGE_PROMPT_Y_POS 0.889583
#define DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE 6
#define DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE 30
#define DSANDGRAINS_SCROLLABLE_LIST_AREA_OFFSET 11
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_X_POS 0.678
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SHADOW_X_POS 0.81875
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SHADOW_Y_POS 0.360416
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_Y_POS 0.35625
#define DSANDGRAINS_INSTANCES_ARROW_UP_X_POS 0.81875
#define DSANDGRAINS_INSTANCES_ARROW_UP_Y_POS 0.404166
#define DSANDGRAINS_INSTANCES_MIN_AREA_X 668.0
#define DSANDGRAINS_INSTANCES_MAX_AREA_X 786.0
#define DSANDGRAINS_INSTANCES_MIN_AREA_Y 148.0
#define DSANDGRAINS_INSTANCES_MAX_AREA_Y 160.0
#define DSANDGRAINS_INSTANCES_ARROW_UP_MIN_AREA_Y 139.0
#define DSANDGRAINS_INSTANCES_ARROW_UP_MAX_AREA_Y 148.0
#define DSANDGRAINS_INSTANCES_ARROW_DOWN_Y_POS 0.04166
#define DSANDGRAINS_INSTANCES_ARROW_DOWN_MIN_AREA_Y 226
#define DSANDGRAINS_INSTANCES_ARROW_DOWN_MAX_AREA_Y 235
#define DSANDGRAINS_VOICE_KNOBS_COUNT 4
#define DSANDGRAINS_VOICE_SCROLLABLE_LIST_Y_POS -0.104166
#define DSANDGRAINS_VOICES_ARROW_UP_Y_POS -0.058333
#define DSANDGRAINS_VOICES_ARROW_UP_MIN_AREA_Y 250
#define DSANDGRAINS_VOICES_ARROW_UP_MAX_AREA_Y 259
#define DSANDGRAINS_VOICES_ARROW_DOWN_Y_POS -0.420833
#define DSANDGRAINS_VOICES_ARROW_DOWN_MIN_AREA_Y 337
#define DSANDGRAINS_VOICES_ARROW_DOWN_MAX_AREA_Y 346
#define DSANDGRAINS_SAMPLE_KNOBS_COUNT 8
#define DSANDGRAINS_SAMPLE_LFO_KNOBS_COUNT 4
#define DSANDGRAINS_SAMPLE_LFO_PITCH_KNOBS_COUNT 4
#define DSANDGRAINS_SAMPLE_AMOUNT_PITCH_KNOBS_COUNT 2
#define DSANDGRAINS_SAMPLES_ARROW_UP_Y_POS -0.520833
#define DSANDGRAINS_SAMPLES_ARROW_UP_MIN_AREA_Y 361
#define DSANDGRAINS_SAMPLES_ARROW_UP_MAX_AREA_Y 370
#define DSANDGRAINS_SAMPLES_ARROW_DOWN_Y_POS -0.883333
#define DSANDGRAINS_SAMPLES_ARROW_DOWN_MIN_AREA_Y 448
#define DSANDGRAINS_SAMPLES_ARROW_DOWN_MAX_AREA_Y 457
#define DSANDGRAINS_SCROLLABLE_LIST_LINE_HEIGHT 11.0
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_INDEX_OFFSET 7
#define DSANDGRAINS_DAHDSR_SLIDERS_COUNT 6
#define DSANDGRAINS_EQUALIZER_SLIDERS_COUNT 8
#define DSANDGRAINS_INSTANCES_ARROWS 6
#define DSANDGRAINS_TINY_BUTTONS_COUNT 1
#define DSANDGRAINS_SCROLLABLE_LIST_SHADOW_WIDTH 117
#define DSANDGRAINS_SCROLLABLE_LIST_SHADOW_HEIGHT 11
#define DSANDGRAINS_SCROLLABLE_LIST_SHADOW_OFFSET 0.045833
#define DSANDGRAINS_TINY_BUTTON_SCALE 32
#define DSANDGRAINS_KNOB1_SCALE 64
#define DSANDGRAINS_KNOB2_SCALE 48
#define DSANDGRAINS_SLIDER1_SCALE 10
#define DSANDGRAINS_SYSTEM_USAGE_WIDTH 30
#define DSANDGRAINS_SYSTEM_USAGE_HEIGHT 23
#define DSANDGRAINS_CHARSET_WIDTH 104
#define DSANDGRAINS_CHARSET_HEIGHT 234
#define DSANDGRAINS_ARROW_BUTTON_WIDTH 117
#define DSANDGRAINS_ARROW_BUTTON_HEIGHT 8
#define DSANDGRAINS_CHARSET_SMALL_WIDTH 52
#define DSANDGRAINS_CHARSET_SMALL_HEIGHT 117
#define DSANDGRAINS_SCROLLABLE_LIST_SIZE 7
#define DSANDGRAINS_SLIDERS_OFFSET 0.04
#define DSANDGRAINS_SLIDERS_SLIDE 42
#define DSANDGRAINS_TEXT_POINTER_WIDTH 1
#define DSANDGRAINS_SMALL_TEXT_POINTER_HEIGHT DSANDGRAINS_CHARSET_SMALL_HEIGHT / 13

#define DSANDGRAINS_BUTTONS_COUNT \
    DSANDGRAINS_INSTANCES_ARROWS + \
    DSANDGRAINS_SCROLLABLE_LIST_SIZE * 2 +/*TODO set to 3*/ \
    DSANDGRAINS_TINY_BUTTONS_COUNT

#define DSANDGRAINS_KNOBS_COUNT \
    DSANDGRAINS_SAMPLE_KNOBS_COUNT + \
    DSANDGRAINS_SAMPLE_LFO_KNOBS_COUNT + \
    DSANDGRAINS_SAMPLE_LFO_PITCH_KNOBS_COUNT + \
    DSANDGRAINS_SAMPLE_AMOUNT_PITCH_KNOBS_COUNT + \
    DSANDGRAINS_VOICE_KNOBS_COUNT
    
#define DSANDGRAINS_UI_ELEMENTS_COUNT \
    DSANDGRAINS_DAHDSR_SLIDERS_COUNT + \
    DSANDGRAINS_EQUALIZER_SLIDERS_COUNT + \
    DSANDGRAINS_KNOBS_COUNT + \
    DSANDGRAINS_BUTTONS_COUNT \

#define DSANDGRAINS_KNOB1_ASSET_PATH        "../assets/knob1.png"
#define DSANDGRAINS_KNOB2_ASSET_PATH        "../assets/knob2.png"
#define DSANDGRAINS_SLIDER1_ASSET_PATH      "../assets/slider1.png"
#define DSANDGRAINS_BACKGROUND_ASSET_PATH   "../assets/dsandgrains_background.png"
#define DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH "../assets/system_usage.png"

void * ui_thread(void * arg);
