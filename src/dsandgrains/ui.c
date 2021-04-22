/*
 * Copyright 2019, 2021 Denis Salem
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

#include <stdlib.h>
#include <time.h>

#include "../buttons.h"
#include "../common.h"
#include "../extensions.h"
#include "../instances.h"
#include "../interactive_list.h"
#include "../open_file.h"
#include "../ressource_usage.h"
#include "../text_pointer.h"
#include "../window_management.h"

#include "add.h"
#include "samples.h"
#include "sample_screen.h"
#include "ui.h"

UIElementsStruct g_ui_elements_struct = {0};
UIElements * g_ui_elements_array = (UIElements *) &g_ui_elements_struct;

// TODO : Shouldn't be be static ?
Vec2 arrow_button_scale_matrix[2] = {0};
Vec2 knob1_64_scale_matrix[2] = {0};
Vec2 knob1_48_scale_matrix[2] = {0};
Vec2 list_item_highlight_scale_matrix[2] = {0};
Vec2 ressource_usage_prompt_scale_matrix[2] = {0};
Vec2 slider1_10_scale_matrix[2] = {0};
Vec2 tiny_button_scale_matrix[2] = {0};
Vec2 sub_menu_buttons_add_scale_matrix[2] = {0};
Vec2 s_instances_slider_scale_matrix[2] = {0};
Vec2 s_sample_screen_sample_scale_matrix[2] = {0};

inline static void bind_callbacks() {
    g_ui_elements_struct.button_add.application_callback = add_sub_menu;
    g_ui_elements_struct.button_add_instance.application_callback = add_instance;
    g_ui_elements_struct.button_add_sample.application_callback = add_sample;
    g_ui_elements_struct.button_add_voice.application_callback = add_voice;
    
    bind_scroll_bar(&g_ui_instances, &g_ui_elements_struct.instances_list_slider);
    bind_scroll_bar(&g_ui_voices, &g_ui_elements_struct.voices_list_slider);
}

inline static void init_background() {
    GLuint texture_ids[2] = {0};
    
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_NONE,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height, 
        DSANDGRAINS_BACKGROUND_ASSET_PATH,
        g_background_scale_matrix,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.background,
        &texture_ids[0],
        &g_background_scale_matrix[0],
        0,
        0,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_TEXTURE_IS_PATTERN | DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_PATTERN_SCALE,
        DSTUDIO_PATTERN_SCALE, 
        DSTUDIO_BACKGROUND_MENU_PATTERN_ASSET_PATH,
        NULL,
        &g_ui_elements_struct.menu_background.pattern_scale
    );
    
    g_ui_elements_struct.menu_background.pattern_scale.width = DSTUDIO_PATTERN_SCALE;
    g_ui_elements_struct.menu_background.pattern_scale.height = DSTUDIO_PATTERN_SCALE;
    
    init_ui_elements(
        &g_ui_elements_struct.menu_background,
        &texture_ids[0],
        &g_background_scale_matrix[0],
        0,
        0,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_PATTERN_BACKGROUND,
        DSTUDIO_FLAG_TEXTURE_IS_PATTERN
    );
    
    init_extended_background(
        DSTUDIO_EXTENDED_BACKGROUND_ASSET_PATH,
        DSTUDIO_EXTENDED_BACKGROUND_SCALE,
        DSTUDIO_EXTENDED_BACKGROUND_SCALE
    ); 
}

inline static void init_instances_list() {
    init_ui_elements(
        &g_ui_elements_struct.instances_list_item_1,
        &g_charset_4x9_texture_ids[0],
        &g_charset_4x9_scale_matrix[0],
        DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_ITEM_POS_X,
        DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_ITEM_POS_Y,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT,
        0,
        DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET,
        1,
        DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE,
        DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    init_ui_elements(
        &g_ui_elements_struct.voices_list_item_1,
        &g_charset_4x9_texture_ids[0],
        &g_charset_4x9_scale_matrix[0],
        DSANDGRAINS_VOICE_SCROLLABLE_LIST_ITEM_POS_X,
        DSANDGRAINS_VOICE_SCROLLABLE_LIST_ITEM_POS_Y,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT,
        0,
        DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET,
        1,
        DSANDGRAINS_VOICE_SCROLLABLE_LIST_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE,
        DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    init_ui_elements(
        &g_ui_elements_struct.samples_list_item_1,
        &g_charset_4x9_texture_ids[0],
        &g_charset_4x9_scale_matrix[0],
        DSANDGRAINS_SAMPLE_SCROLLABLE_LIST_ITEM_POS_X,
        DSANDGRAINS_SAMPLE_SCROLLABLE_LIST_ITEM_POS_Y,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT,
        0,
        DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET,
        1,
        DSANDGRAINS_SAMPLE_SCROLLABLE_LIST_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE,
        DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM,
        DSTUDIO_FLAG_IS_VISIBLE
    );
}

inline static void init_knobs() {
    GLuint knob_texture_ids[2] = {0};
        
    knob_texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING,
        DSTUDIO_KNOB_1_64_WIDTH,
        DSTUDIO_KNOB_1_64_HEIGHT, 
        DSTUDIO_KNOB_1_64x64_TEXTURE_PATH,
        knob1_64_scale_matrix,
        NULL
    );

    init_ui_elements(
        &g_ui_elements_struct.knob_sample_start,
        &knob_texture_ids[0],
        &knob1_64_scale_matrix[0],
        DSANDGRAINS_SAMPLE_KNOBS_POS_X,
        DSANDGRAINS_SAMPLE_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_64_AREA_WIDTH,
        DSTUDIO_KNOB_1_64_AREA_HEIGHT,
        DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X,
        DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y,
        DSANDGRAINS_SAMPLE_KNOBS_COLUMNS,
        12,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    init_ui_elements(
        &g_ui_elements_struct.knob_sample_pitch,
        &knob_texture_ids[0],
        &knob1_64_scale_matrix[0],
        DSANDGRAINS_RANDOMIZE_N_SAMPLE_PITCH_KNOBS_POS_X,
        DSANDGRAINS_RANDOMIZE_N_SAMPLE_PITCH_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_64_AREA_WIDTH,
        DSTUDIO_KNOB_1_64_AREA_HEIGHT,
        DSANDGRAINS_RANDOMIZE_N_SAMPLE_PITCH_KNOBS_OFFSET_X,
        DSANDGRAINS_RANDOMIZE_N_SAMPLE_PITCH_KNOBS_OFFSET_Y,
        DSANDGRAINS_RANDOMIZE_N_SAMPLE_PITCH_KNOBS_COLUMNS,
        2,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    knob_texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING,
        DSTUDIO_KNOB_1_48_WIDTH,
        DSTUDIO_KNOB_1_48_HEIGHT, 
        DSTUDIO_KNOB_1_48x48_TEXTURE_PATH,
        knob1_48_scale_matrix,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.knob_voice_volume,
        &knob_texture_ids[0],
        &knob1_48_scale_matrix[0],
        DSANDGRAINS_VOICE_KNOBS_POS_X,
        DSANDGRAINS_VOICE_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_48_AREA_WIDTH,
        DSTUDIO_KNOB_1_48_AREA_HEIGHT,
        DSANDGRAINS_VOICE_KNOBS_OFFSET_X,
        DSANDGRAINS_VOICE_KNOBS_OFFSET_Y,
        DSANDGRAINS_VOICE_KNOBS_COLUMNS,
        4,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    init_ui_elements(
        &g_ui_elements_struct.knob_lfo_tune,
        &knob_texture_ids[0],
        &knob1_48_scale_matrix[0],
        DSANDGRAINS_LFO_KNOBS_POS_X,
        DSANDGRAINS_LFO_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_48_AREA_WIDTH,
        DSTUDIO_KNOB_1_48_AREA_HEIGHT,
        DSANDGRAINS_LFO_KNOBS_OFFSET_X,
        DSANDGRAINS_LFO_KNOBS_OFFSET_Y,
        DSANDGRAINS_LFO_KNOBS_COLUMNS,
        4,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB,
        DSTUDIO_FLAG_IS_VISIBLE
    );
}

inline static void init_list_item_highlights() {
    GLuint texture_ids[2] = {0};
    
    DEFINE_SCALE_MATRIX(
        list_item_highlight_scale_matrix,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT
    )
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_TEXTURE_IS_PATTERN,
        DSTUDIO_PATTERN_SCALE,
        DSTUDIO_PATTERN_SCALE, 
        DSTUDIO_LIST_ITEM_HIGHLIGHT_PATTERN_PATH,
        NULL,
        &g_ui_elements_struct.instances_list_item_highlight.pattern_scale
    );
    
    init_ui_elements(
        &g_ui_elements_struct.instances_list_item_highlight,
        &texture_ids[0],
        &list_item_highlight_scale_matrix[0],
        DSANDGRAINS_INSTANCE_ITEM_LIST_HIGHLIGHT_POS_X,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HIGHLIGHT_POS_Y,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT, 
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_FLAG_TEXTURE_IS_PATTERN
    );
    
    init_ui_elements(
        &g_ui_elements_struct.voices_list_item_highlight,
        &texture_ids[0],
        &list_item_highlight_scale_matrix[0],
        DSANDGRAINS_VOICE_ITEM_LIST_HIGHLIGHT_POS_X,
        DSANDGRAINS_VOICE_ITEM_LIST_HIGHLIGHT_POS_Y,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT, 
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_FLAG_TEXTURE_IS_PATTERN
    );
    
    init_ui_elements(
        &g_ui_elements_struct.samples_list_item_highlight,
        &texture_ids[0],
        &list_item_highlight_scale_matrix[0],
        DSANDGRAINS_SAMPLE_ITEM_LIST_HIGHLIGHT_POS_X,
        DSANDGRAINS_SAMPLE_ITEM_LIST_HIGHLIGHT_POS_Y,
        DSANDGRAINS_INSTANCE_ITEM_LIST_WIDTH,
        DSANDGRAINS_INSTANCE_ITEM_LIST_HEIGHT, 
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_FLAG_TEXTURE_IS_PATTERN
    );
}

inline static void init_misc_buttons() {
    GLuint textures_ids[2] = {0};
    
    textures_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSANDGRAINS_TINY_BUTTON_SCALE,
        DSANDGRAINS_TINY_BUTTON_SCALE, 
        DSTUDIO_BUTTON_ADD_ASSET_PATH,
        tiny_button_scale_matrix,
        NULL
    );
    
    textures_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSANDGRAINS_TINY_BUTTON_SCALE,
        DSANDGRAINS_TINY_BUTTON_SCALE, 
        DSTUDIO_ACTIVE_BUTTON_ADD_ASSET_PATH,
        NULL,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.button_add,
        &textures_ids[0],
        &tiny_button_scale_matrix[0],
        DSANDGRAINS_ADD_BUTTON_POS_X,
        DSANDGRAINS_ADD_BUTTON_POS_Y,
        DSANDGRAINS_TINY_BUTTON_SCALE,
        DSANDGRAINS_TINY_BUTTON_SCALE,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE,
        DSTUDIO_FLAG_IS_VISIBLE
    );
}

inline static void init_ressource_usage() {
    GLuint textures_ids[2] = {0};
        
    textures_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_RESSOURCE_USAGE_WIDTH,
        DSTUDIO_RESSOURCE_USAGE_HEIGHT, 
        DSTUDIO_RESSOURCE_USAGE_PROMPT_ASSET_PATH,
        ressource_usage_prompt_scale_matrix,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.ressource_usage_prompt,
        &textures_ids[0],
        &ressource_usage_prompt_scale_matrix[0],
        DSANDGRAINS_RESSOURCE_USAGE_PROMPT_POS_X,
        DSANDGRAINS_RESSOURCE_USAGE_PROMPT_POS_Y,
        DSTUDIO_RESSOURCE_USAGE_WIDTH,
        DSTUDIO_RESSOURCE_USAGE_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND,
        DSTUDIO_FLAG_IS_VISIBLE
    );

    // Initialize both cpu_usage and memory usage.
    init_ui_elements(
        &g_ui_elements_struct.cpu_usage,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        DSANDGRAINS_CPU_N_MEM_USAGE_X_POS,
        DSANDGRAINS_CPU_USAGE_Y_POS,
        DSANDGRAINS_CPU_USAGE_WIDTH,
        DSANDGRAINS_CPU_USAGE_HEIGHT,
        0,
        DSANDGRAINS_MEM_USAGE_OFFSET_Y,
        1,
        2,
        DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    // TODO Finish Implement this.
    g_ui_elements_struct.cpu_usage.overlap_sub_menu_ui_elements = &g_ui_elements_struct.open_file_menu_prompts;
}

inline static void init_sample_screen() {    
          
    DEFINE_SCALE_MATRIX(
        s_sample_screen_sample_scale_matrix,
        DSANDGRAINS_SAMPLE_SCREEN_SAMPLE_WIDTH,
        DSANDGRAINS_SAMPLE_SCREEN_HEIGHT
    )
    
    init_ui_elements(
        &g_ui_elements_struct.sample_screen,
        NULL,
        &s_sample_screen_sample_scale_matrix[0],
        DSANDGRAINS_SAMPLE_SCREEN_POS_X, 
        DSANDGRAINS_SAMPLE_SCREEN_POS_Y,
        DSANDGRAINS_SAMPLE_SCREEN_SAMPLE_WIDTH,
        DSANDGRAINS_SAMPLE_SCREEN_HEIGHT,
        DSANDGRAINS_SAMPLE_SCREEN_OFFSET_X,
        DSANDGRAINS_SAMPLE_SCREEN_OFFSET_Y,
        1,
        1,
        DSANDGRAINS_SAMPLE_SCREEN_WIDTH,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BAR_PLOT,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_ANIMATE_MOTION | DSTUDIO_ANIMATE_OFFSET
    );
    
    g_ui_elements_struct.sample_screen.color.r = 1.0;
    g_ui_elements_struct.sample_screen.color.g = 0.5;
    g_ui_elements_struct.sample_screen.color.a = 1;
}

inline static void init_sliders() {
    GLuint slider_texture_ids[2] = {0};
        
    slider_texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_SLIDER_1_10_WIDTH,
        DSTUDIO_SLIDER_1_10_HEIGHT, 
        DSTUDIO_SLIDER_1_10x10_TEXTURE_PATH,
        slider1_10_scale_matrix,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.slider_delay,
        &slider_texture_ids[0],
        &slider1_10_scale_matrix[0],
        DSANDGRAINS_DAHDSR_SLIDERS_POS_X,
        DSANDGRAINS_DAHDSR_SLIDERS_POS_Y,
        DSTUDIO_SLIDER_1_10_AREA_WIDTH,
        DSTUDIO_SLIDER_1_10_AREA_HEIGHT,
        DSANDGRAINS_DAHDSR_SLIDERS_OFFSET_X,
        DSANDGRAINS_DAHDSR_SLIDERS_OFFSET_Y,
        DSANDGRAINS_DAHDSR_SLIDERS_COLUMNS,
        DSANDGRAINS_DAHDSR_SLIDERS_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER,
        DSTUDIO_FLAG_IS_VISIBLE
    );
    
    init_ui_elements(
        &g_ui_elements_struct.slider_equalizer_Q,
        &slider_texture_ids[0],
        &slider1_10_scale_matrix[0],
        DSANDGRAINS_EQUALIZER_SLIDERS_POS_X,
        DSANDGRAINS_EQUALIZER_SLIDERS_POS_Y,
        DSTUDIO_SLIDER_1_10_AREA_WIDTH,
        DSTUDIO_SLIDER_1_10_AREA_HEIGHT,
        DSANDGRAINS_EQUALIZER_SLIDERS_OFFSET_X,
        DSANDGRAINS_EQUALIZER_SLIDERS_OFFSET_Y,
        DSANDGRAINS_EQUALIZER_SLIDERS_COLUMNS,
        DSANDGRAINS_EQUALIZER_SLIDERS_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER,
        DSTUDIO_FLAG_IS_VISIBLE
    );

    slider_texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_SLIDER_2_8_WIDTH,
        DSTUDIO_SLIDER_2_8_HEIGHT, 
        DSTUDIO_SLIDER_2_8x8_TEXTURE_PATH,
        s_instances_slider_scale_matrix,
        NULL
    );

    init_ui_elements(
        &g_ui_elements_struct.instances_list_slider,
        &slider_texture_ids[0],
        &s_instances_slider_scale_matrix[0],
        DSANDGRAINS_INSTANCE_SLIDER_POS_X,
        DSANDGRAINS_INSTANCE_SLIDER_POS_Y,
        DSTUDIO_SLIDER_2_8_AREA_WIDTH,
        DSTUDIO_SLIDER_2_8_AREA_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_FLAG_SLIDER_TO_TOP
    );
    
    init_ui_elements(
        &g_ui_elements_struct.voices_list_slider,
        &slider_texture_ids[0],
        &s_instances_slider_scale_matrix[0],
        DSANDGRAINS_VOICE_SLIDER_POS_X,
        DSANDGRAINS_VOICE_SLIDER_POS_Y,
        DSTUDIO_SLIDER_2_8_AREA_WIDTH,
        DSTUDIO_SLIDER_2_8_AREA_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_FLAG_SLIDER_TO_TOP
    );
    
    init_ui_elements(
        &g_ui_elements_struct.samples_list_slider,
        &slider_texture_ids[0],
        &s_instances_slider_scale_matrix[0],
        DSANDGRAINS_SAMPLE_SLIDER_POS_X,
        DSANDGRAINS_SAMPLE_SLIDER_POS_Y,
        DSTUDIO_SLIDER_2_8_AREA_WIDTH,
        DSTUDIO_SLIDER_2_8_AREA_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER,
        DSTUDIO_FLAG_IS_VISIBLE | DSTUDIO_FLAG_SLIDER_TO_TOP
    );
}

inline static void init_sub_menu_add() {
    GLuint texture_ids[2] = {0};
        
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_ADD_INSTANCE_WIDTH,
        DSTUDIO_ADD_INSTANCE_HEIGHT, 
        DSTUDIO_ADD_INSTANCE_ASSET_PATH,
        sub_menu_buttons_add_scale_matrix,
        NULL
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_ADD_INSTANCE_WIDTH,
        DSTUDIO_ADD_INSTANCE_HEIGHT, 
        DSTUDIO_ACTIVE_ADD_INSTANCE_ASSET_PATH,
        NULL,
        NULL
    );

    init_ui_elements(
        &g_ui_elements_struct.button_add_instance,
        &texture_ids[0],
        &sub_menu_buttons_add_scale_matrix[0],
        DSANDGRAINS_ADD_INSTANCE_POS_X,
        DSANDGRAINS_ADD_INSTANCE_POS_Y,
        DSTUDIO_ADD_INSTANCE_WIDTH,
        DSTUDIO_ADD_INSTANCE_HEIGHT,
        DSANDGRAINS_ADD_INSTANCE_OFFSET_X,
        DSANDGRAINS_ADD_INSTANCE_OFFSET_Y,
        DSANDGRAINS_ADD_INSTANCE_COLUMNS,
        DSANDGRAINS_ADD_INSTANCE_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON,
        DSTUDIO_FLAG_NONE
    );
    
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_ADD_VOICE_WIDTH,
        DSTUDIO_ADD_VOICE_HEIGHT, 
        DSTUDIO_ADD_VOICE_ASSET_PATH,
        sub_menu_buttons_add_scale_matrix,
        NULL
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_ADD_INSTANCE_WIDTH,
        DSTUDIO_ADD_INSTANCE_HEIGHT,
        DSTUDIO_ACTIVE_ADD_VOICE_ASSET_PATH,
        NULL,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.button_add_voice,
        &texture_ids[0],
        &sub_menu_buttons_add_scale_matrix[0],
        DSANDGRAINS_ADD_VOICE_POS_X,
        DSANDGRAINS_ADD_VOICE_POS_Y,
        DSTUDIO_ADD_VOICE_WIDTH,
        DSTUDIO_ADD_VOICE_HEIGHT,
        DSANDGRAINS_ADD_VOICE_OFFSET_X,
        DSANDGRAINS_ADD_VOICE_OFFSET_Y,
        DSANDGRAINS_ADD_VOICE_COLUMNS,
        DSANDGRAINS_ADD_VOICE_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON,
        DSTUDIO_FLAG_NONE
    );
    
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_ADD_SAMPLE_WIDTH,
        DSTUDIO_ADD_SAMPLE_HEIGHT, 
        DSTUDIO_ADD_SAMPLE_ASSET_PATH,
        sub_menu_buttons_add_scale_matrix,
        NULL
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_ADD_INSTANCE_WIDTH,
        DSTUDIO_ADD_INSTANCE_HEIGHT, 
        DSTUDIO_ACTIVE_ADD_SAMPLE_ASSET_PATH,
        NULL,
        NULL
    );
    
    init_ui_elements(
        &g_ui_elements_struct.button_add_sample,
        &texture_ids[0],
        &sub_menu_buttons_add_scale_matrix[0],
        DSANDGRAINS_ADD_SAMPLE_POS_X,
        DSANDGRAINS_ADD_SAMPLE_POS_Y,
        DSTUDIO_ADD_SAMPLE_WIDTH,
        DSTUDIO_ADD_SAMPLE_HEIGHT,
        DSANDGRAINS_ADD_SAMPLE_OFFSET_X,
        DSANDGRAINS_ADD_SAMPLE_OFFSET_Y,
        DSANDGRAINS_ADD_SAMPLE_COLUMNS,
        DSANDGRAINS_ADD_SAMPLE_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON,
        DSTUDIO_FLAG_NONE
    );
    
    init_ui_elements(
        &g_ui_elements_struct.add_sub_menu_prompt,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        DSANDGRAINS_ADD_SUB_MENU_PROMPT_POS_X,
        DSANDGRAINS_ADD_SUB_MENU_PROMPT_POS_Y,
        DSANDGRAINS_ADD_SUB_MENU_PROMPT_WIDTH,
        DSANDGRAINS_ADD_SUB_MENU_PROMPT_HEIGHT,
        0,
        0,
        DSANDGRAINS_ADD_SUB_MENU_COLUMNS,
        DSANDGRAINS_ADD_SUB_MENU_COUNT,
        29,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT_BACKGROUND,
        DSTUDIO_FLAG_NONE
    );
    update_text(&g_ui_elements_struct.add_sub_menu_prompt, "PICK THE ITEM YOU WANT TO ADD", 29);
    update_gpu_buffer(&g_ui_elements_struct.add_sub_menu_prompt);
    g_ui_elements_struct.add_sub_menu_prompt.render_state = DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED;
    
}

static void init_dsandgrains_ui_elements() {
    g_scale_matrix_id = glGetUniformLocation(g_shader_program_id, "scale_matrix");
    init_text();
    init_list_item_highlights();

    init_background();

    init_buttons_management();
    init_instances_list();
    init_knobs();
    init_misc_buttons();
    init_ressource_usage();
    init_sliders();
    init_sample_screen();
    init_ui_text_pointer(&g_ui_elements_struct.text_pointer);
    init_sub_menu_add();
    init_open_menu(
        &g_ui_elements_struct.menu_background,
        &g_ui_elements_struct.open_file_menu_prompts_box
    );
    
    bind_callbacks();
    for (unsigned int i = 4; i < g_menu_background_index; i++) {
        g_ui_elements_array[i].enabled = 1;
    }

}

void * ui_thread(void * arg) {
    (void) arg;
    
    SET_UI_MENU_BACKGROUND_INDEX
    
    init_ui();
    init_dsandgrains_ui_elements();

    init_ressource_usage_backend(
        DSTUDIO_RESSOURCE_USAGE_STRING_SIZE,
        &g_ui_elements_struct.cpu_usage,
        &g_ui_elements_struct.mem_usage
    );
    init_instance_management_backend();

    init_instances_interactive_list(
        &g_ui_elements_struct.instances_list_item_highlight,
        DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET
    );

    init_voices_interactive_list(
        &g_ui_elements_struct.voices_list_item_highlight,
        DSANDGRAINS_VOICE_SCROLLABLE_LIST_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET
    );
    
    init_samples_interactive_list(
        &g_ui_elements_struct.samples_list_item_highlight,
        DSANDGRAINS_VOICE_SCROLLABLE_LIST_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE,
        DSANDGRAINS_SCROLLABLE_LIST_ITEM_OFFSET
    );
    
    bind_scroll_bar(&g_ui_samples, &g_ui_elements_struct.samples_list_slider);

    bind_voices_interactive_list(NULL);
    
    bind_samples_interactive_list(NULL, DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK);
    
    init_ui_element_updater_register(8);
    
    register_ui_elements_updater(text_pointer_blink);
    register_ui_elements_updater(update_ui_ressource_usage);
    register_ui_elements_updater(update_ui_bouncing_buttons);
    register_ui_elements_updater(update_ui_instances_list);

    register_ui_elements_updater(update_voices_ui_list);
    register_ui_elements_updater(update_open_file_ui_list);
    register_ui_elements_updater(update_samples_ui_list);
    register_ui_elements_updater(update_sample_screen);

    render_loop();
    destroy_context();
    return NULL;
}
