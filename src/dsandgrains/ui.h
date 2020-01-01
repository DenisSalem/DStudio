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

#define DSANDGRAINS_SAMPLE_KNOBS_POS_X -0.8675
#define DSANDGRAINS_SAMPLE_KNOBS_POS_Y 0.25
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X 0.16
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y -0.4
#define DSANDGRAINS_SAMPLE_KNOBS_COLUMNS 4

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
} UIElementsArray;

void * ui_thread(void * arg);
