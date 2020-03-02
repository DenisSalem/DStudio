/*
 * Copyright 2019, 2020 Denis Salem
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

#define DSANDGRAINS_ADD_BUTTON_POS_X 0.2175
#define DSANDGRAINS_ADD_BUTTON_POS_Y -0.6625

#define DSANDGRAINS_ADD_INSTANCE_COLUMNS 1
#define DSANDGRAINS_ADD_INSTANCE_COUNT 1
#define DSANDGRAINS_ADD_INSTANCE_OFFSET_X 0
#define DSANDGRAINS_ADD_INSTANCE_OFFSET_Y 0
#define DSANDGRAINS_ADD_INSTANCE_POS_X -0.5
#define DSANDGRAINS_ADD_INSTANCE_POS_Y -0.335333

#define DSANDGRAINS_ADD_SAMPLE_COLUMNS 1
#define DSANDGRAINS_ADD_SAMPLE_COUNT 1
#define DSANDGRAINS_ADD_SAMPLE_OFFSET_X 0
#define DSANDGRAINS_ADD_SAMPLE_OFFSET_Y 0
#define DSANDGRAINS_ADD_SAMPLE_POS_X 0.5
#define DSANDGRAINS_ADD_SAMPLE_POS_Y -0.335333

#define DSANDGRAINS_ADD_SUB_MENU_COLUMNS 1
#define DSANDGRAINS_ADD_SUB_MENU_COUNT 1
#define DSANDGRAINS_ADD_SUB_MENU_PROMPT_POS_X -0.279999
#define DSANDGRAINS_ADD_SUB_MENU_PROMPT_POS_Y 0.333333
#define DSANDGRAINS_ADD_SUB_MENU_PROMPT_HEIGHT 18
#define DSANDGRAINS_ADD_SUB_MENU_PROMPT_WIDTH 233

#define DSANDGRAINS_ADD_VOICE_COLUMNS 1
#define DSANDGRAINS_ADD_VOICE_COUNT 1
#define DSANDGRAINS_ADD_VOICE_OFFSET_X 0
#define DSANDGRAINS_ADD_VOICE_OFFSET_Y 0
#define DSANDGRAINS_ADD_VOICE_POS_X 0
#define DSANDGRAINS_ADD_VOICE_POS_Y -0.335333
        
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_X 0.13
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_Y 0
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_X 0.0375
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_Y 0.3
#define DSANDGRAINS_AMOUNT_PITCH_KNOBS_COLUMNS 2

#define DSANDGRAINS_ARROW_BUTTON_WIDTH 117
#define DSANDGRAINS_ARROW_BUTTON_HEIGHT 8

#define DSANDGRAINS_BACKGROUND_ASSET_PATH "../assets/dsandgrains_background.png"

#define DSANDGRAINS_CPU_N_MEM_USAGE_X_POS 0.015

#define DSANDGRAINS_CPU_USAGE_WIDTH 48
#define DSANDGRAINS_CPU_USAGE_HEIGHT 10
#define DSANDGRAINS_CPU_USAGE_Y_POS 0.921875

#define DSANDGRAINS_DAHDSR_SLIDERS_COLUMNS 6
#define DSANDGRAINS_DAHDSR_SLIDERS_COUNT 6
#define DSANDGRAINS_DAHDSR_SLIDERS_OFFSET_X 0.04
#define DSANDGRAINS_DAHDSR_SLIDERS_OFFSET_Y 0
#define DSANDGRAINS_DAHDSR_SLIDERS_POS_X 0.0275
#define DSANDGRAINS_DAHDSR_SLIDERS_POS_Y -0.466666
#define DSANDGRAINS_DAHDSR_SLIDERS_SLIDE 42

#define DSANDGRAINS_EQUALIZER_SLIDERS_COLUMNS 8
#define DSANDGRAINS_EQUALIZER_SLIDERS_COUNT 8
#define DSANDGRAINS_EQUALIZER_SLIDERS_OFFSET_X 0.04
#define DSANDGRAINS_EQUALIZER_SLIDERS_OFFSET_Y 0
#define DSANDGRAINS_EQUALIZER_SLIDERS_POS_X 0.32
#define DSANDGRAINS_EQUALIZER_SLIDERS_POS_Y -0.425

#define DSANDGRAINS_INSTANCE_ITEM_LIST_HIGHLIGHT_POS_X 0.8195
#define DSANDGRAINS_INSTANCE_ITEM_LIST_HIGHLIGHT_POS_Y 0.35625
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_ITEM_POS_X 0.678
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_ITEM_POS_Y 0.35625
#define DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SIZE 7

#define DSANDGRAINS_INSTANCES_ARROW_DOWN_X_POS 0.81875
#define DSANDGRAINS_INSTANCES_ARROW_DOWN_Y_POS 0.04167
#define DSANDGRAINS_INSTANCES_ARROW_UP_POS_X 0.81875
#define DSANDGRAINS_INSTANCES_ARROW_UP_POS_Y 0.404167
#define DSANDGRAINS_INSTANCES_OFFSET_X 0
#define DSANDGRAINS_INSTANCES_OFFSET_Y -0.4625

#define DSANDGRAINS_ITEM_LIST_WIDTH 117
#define DSANDGRAINS_ITEM_LIST_HEIGHT 11

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

#define DSANDGRAINS_MEM_USAGE_OFFSET_Y -0.05078125

#define DSANDGRAINS_RESSOURCE_USAGE_PROMPT_POS_X -0.035
#define DSANDGRAINS_RESSOURCE_USAGE_PROMPT_POS_Y 0.8984375
#define DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE 6

#define DSANDGRAINS_SAMPLE_KNOBS_COLUMNS 4
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X 0.16
#define DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y -0.4
#define DSANDGRAINS_SAMPLE_KNOBS_POS_X -0.8675
#define DSANDGRAINS_SAMPLE_KNOBS_POS_Y 0.25

#define DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET -0.045833
#define DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE 30

#define DSANDGRAINS_VOICE_KNOBS_COLUMNS 2
#define DSANDGRAINS_VOICE_KNOBS_OFFSET_X 0.175
#define DSANDGRAINS_VOICE_KNOBS_OFFSET_Y -0.3416
#define DSANDGRAINS_VOICE_KNOBS_POS_X 0.3725
#define DSANDGRAINS_VOICE_KNOBS_POS_Y 0.2708

#define DSANDGRAINS_VOICE_ITEM_LIST_HIGHLIGHT_POS_X 0.8195
#define DSANDGRAINS_VOICE_ITEM_LIST_HIGHLIGHT_POS_Y -0.10625
#define DSANDGRAINS_VOICE_SCROLLABLE_LIST_ITEM_POS_X 0.678
#define DSANDGRAINS_VOICE_SCROLLABLE_LIST_ITEM_POS_Y -0.10625
#define DSANDGRAINS_VOICE_SCROLLABLE_LIST_SIZE 7

typedef struct UIElementsStruct_t {
    UIElements background;
    UIElements ressource_usage_prompt;
    UIElements cpu_usage;
    UIElements mem_usage;
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
    UIElements slider_delay;
    UIElements slider_attack;
    UIElements slider_hold;
    UIElements slider_decay;
    UIElements slider_sustain;
    UIElements slider_release;
    UIElements slider_equalizer_band_1;
    UIElements slider_equalizer_band_2;
    UIElements slider_equalizer_band_3;
    UIElements slider_equalizer_band_4;
    UIElements slider_equalizer_band_5;
    UIElements slider_equalizer_band_6;
    UIElements slider_equalizer_band_7;
    UIElements slider_equalizer_band_8;
    UIElements button_arrow_top_instances;
    UIElements button_arrow_top_voices;
    UIElements button_arrow_top_samples;
    UIElements button_arrow_bottom_instances;
    UIElements button_arrow_bottom_voices;
    UIElements button_arrow_bottom_samples;
    UIElements button_add;
    UIElements text_pointer;
    UIElements instances_list_item_highlight;
    UIElements instances_list_item_1;
    UIElements instances_list_item_2;
    UIElements instances_list_item_3;
    UIElements instances_list_item_4;
    UIElements instances_list_item_5;
    UIElements instances_list_item_6;
    UIElements instances_list_item_7;
    UIElements voices_list_item_highlight;
    UIElements voices_list_item_1;
    UIElements voices_list_item_2;
    UIElements voices_list_item_3;
    UIElements voices_list_item_4;
    UIElements voices_list_item_5;
    UIElements voices_list_item_6;
    UIElements voices_list_item_7;
    //~ UIElements samples_list_item_highlight;
    //~ UIElements dahdsr_target_list_item_highlight;
    UIElements menu_background;
    UIElements add_sub_menu_prompt;
    UIElements button_add_instance;
    UIElements button_add_voice;
    UIElements button_add_sample;
    UIElements open_file_menu_prompts_box;
    UIElements open_file_menu_prompts;
    UIElements open_file_buttons_box;
    UIElements open_file_button_cancel;
    UIElements open_file_button_open;
    UIElements open_file_list_box;
    UIElements open_file_slider_background;
    UIElements open_file_slider;
    UIElements open_file_list_highlight;
    UIElements open_file_list_item_1;
    UIElements open_file_list_item_2;
    UIElements open_file_list_item_3;
    UIElements open_file_list_item_4;
    UIElements open_file_list_item_5;
    UIElements open_file_list_item_6;
    UIElements open_file_list_item_7;
    UIElements open_file_list_item_8;
    UIElements open_file_list_item_9;
    UIElements open_file_list_item_10;
    UIElements open_file_list_item_11;
    UIElements open_file_list_item_12;
    UIElements open_file_list_item_13;
    UIElements open_file_list_item_14;
    UIElements open_file_list_item_15;
    UIElements open_file_list_item_16;
    UIElements open_file_list_item_17;
    UIElements open_file_list_item_18;
    UIElements open_file_list_item_19;
    UIElements open_file_list_item_20;
    UIElements open_file_list_item_21;
    UIElements open_file_list_item_22;
} UIElementsStruct;

void * ui_thread(void * arg);
