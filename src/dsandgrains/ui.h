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

#include "../ui.h"

#define DSANDGRAINS_BACKGROUND_ASSET_PATH "../assets/dsandgrains_background.png"

#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_X 0.13
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_Y 0
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_X 0.0375
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_Y 0.3
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_COLUMNS 2

#define DSANDGRAINS_LFO_KNOBS_COLUMNS 2
#define DSANDGRAINS_LFO_KNOBS_OFFSET_X 0.12
#define DSANDGRAINS_LFO_KNOBS_OFFSET_Y -0.2483
#define DSANDGRAINS_LFO_KNOBS_POS_X -0.2225
#define DSANDGRAINS_LFO_KNOBS_POS_Y 0.25

#define DSANDGRAINS_LFO_PITCH_KNOBS_POS_X -0.2225
#define DSANDGRAINS_LFO_PITCH_KNOBS_POS_Y -0.5125
#define DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_X 0.12
#define DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_Y -0.2483
#define DSANDGRAINS_LFO_PITCH_KNOBS_COLUMNS 2

#define DSANDGRAINS_SAMPLE_KNOBS_COLUMNS 4
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X 0.16
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y -0.4
#define DSANDGRAINS_SAMPLE_KNOBS_POS_X -0.8675
#define DSANDGRAINS_SAMPLE_KNOBS_POS_Y 0.25

#define DSANDGRAINS_VOICE_KNOBS_COLUMNS 2
#define DSANDGRAINS_VOICE_KNOBS_OFFSET_X 0.175
#define DSANDGRAINS_VOICE_KNOBS_OFFSET_Y -0.3416
#define DSANDGRAINS_VOICE_KNOBS_POS_X 0.3725
#define DSANDGRAINS_VOICE_KNOBS_POS_Y 0.2708

typedef struct UIElementsArray_t {
    UIElements background;
    UIElements knob_sample_start;
    UIElements knob_sample_end;
    UIElements knob_sample_grain_size;
    UIElements knob_sample_transient_thresold;
    UIElements knob_sample_loop_duration;
    UIElements knob_sample_cloud_size;
    UIElements knob_sample_distribution_balance;
    UIElements knob_sample_smooth_transition;
    UIElements knob_sample_amount;
    UIElements knob_sample_pitch;
    UIElements knob_voice_volume;
    UIElements knob_voice_pan;
    UIElements knob_voice_influence;
    UIElements knob_voice_density;
    UIElements knob_sample_lfo_tune;
    UIElements knob_sample_lfo_phase;
    UIElements knob_sample_lfo_depth;
    UIElements knob_sample_lfo_signal;
    UIElements knob_sample_lfo_pitch_tune;
    UIElements knob_sample_lfo_pitch_phase;
    UIElements knob_sample_lfo_pitch_depth;
    UIElements knob_sample_lfo_pitch_signal;
} UIElementsArray;

void * ui_thread(void * arg);
