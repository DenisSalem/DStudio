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

#define LOAD_SHARED_TEXTURE \
    background_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_NONE, \
        DSTUDIO_VIEWPORT_WIDTH, \
        DSTUDIO_VIEWPORT_HEIGHT, \
        DSANDGRAINS_BACKGROUND_ASSET_PATH, \
        background_scale_matrix \
    ); \
    knob1_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_KNOB1_ASSET_PATH, \
        knob1_scale_matrix \
    ); \
    knob2_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_ASSET_PATH, \
        knob2_scale_matrix \
    ); \
    slider_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDER1_ASSET_PATH, \
        slider_scale_matrix \
    ); \
    system_usage_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_SYSTEM_USAGE_WIDTH, \
        DSANDGRAINS_SYSTEM_USAGE_HEIGHT, \
        DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH, \
        system_usage_scale_matrix \
    ); \
    charset_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_CHARSET_WIDTH, \
        DSANDGRAINS_CHARSET_HEIGHT, \
        DSTUDIO_CHAR_TABLE_ASSET_PATH, \
        NULL \
    ); \
    charset_small_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_CHARSET_SMALL_WIDTH, \
        DSANDGRAINS_CHARSET_SMALL_HEIGHT, \
        DSTUDIO_CHAR_TABLE_SMALL_ASSET_PATH, \
        NULL \
    );

#define PREPARE_SHARED_SCALE_MATRICES \
    DSTUDIO_SET_UI_TEXT_SCALE_MATRIX( \
        charset_scale_matrix, \
        DSANDGRAINS_CHARSET_WIDTH, \
        DSANDGRAINS_CHARSET_HEIGHT \
    ) \
    DSTUDIO_SET_UI_TEXT_SCALE_MATRIX( \
        charset_small_scale_matrix, \
        DSANDGRAINS_CHARSET_SMALL_WIDTH, \
        DSANDGRAINS_CHARSET_SMALL_HEIGHT \
    ) \
    DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX( \
        scrollable_list_shadow_scale_matrix, \
        DSANDGRAINS_SCROLLABLE_LIST_SHADOW_WIDTH, \
        DSANDGRAINS_SCROLLABLE_LIST_SHADOW_HEIGHT \
    ) \
    DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX( \
        text_pointer_scale_matrix, \
        DSANDGRAINS_TEXT_POINTER_WIDTH, \
        DSANDGRAINS_SMALL_TEXT_POINTER_HEIGHT \
    )

#define SETUP_BUTTONS_SETTING_ARRAYS \
    button_settings_array[0].release = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_ARROW_BUTTON_WIDTH, \
        DSANDGRAINS_ARROW_BUTTON_HEIGHT, \
        DSTUDIO_ARROW_INSTANCES_ASSET_PATH, \
        arrow_instances_scale_matrix \
    ); \
    button_settings_array[0].active = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_ARROW_BUTTON_WIDTH, \
        DSANDGRAINS_ARROW_BUTTON_HEIGHT, \
        DSTUDIO_ACTIVE_ARROW_INSTANCES_ASSET_PATH, \
        NULL \
    ); \
    button_settings_array[0].application_callback = scroll_instances; \
    button_settings_array[0].flags = DSTUDIO_BUTTON_ACTION_LIST_BACKWARD; \
    button_settings_array[1].release = button_settings_array[0].release; \
    button_settings_array[1].active = button_settings_array[0].active; \
    button_settings_array[1].application_callback = scroll_instances; \
    button_settings_array[2].release = button_settings_array[0].release; \
    button_settings_array[2].active = button_settings_array[0].active; \
    button_settings_array[2].application_callback = scroll_voices; \
    button_settings_array[2].flags = DSTUDIO_BUTTON_ACTION_LIST_BACKWARD; \
    button_settings_array[3].release = button_settings_array[0].release; \
    button_settings_array[3].active = button_settings_array[0].active; \
    button_settings_array[3].application_callback = scroll_voices; \
    button_settings_array[4].release = button_settings_array[0].release; \
    button_settings_array[4].active = button_settings_array[0].active; \
    button_settings_array[4].application_callback = scroll_voices; \
    button_settings_array[4].flags = DSTUDIO_BUTTON_ACTION_LIST_BACKWARD; \
    button_settings_array[5].release = button_settings_array[0].release; \
    button_settings_array[5].active = button_settings_array[0].active; \
    button_settings_array[5].application_callback = scroll_voices; \
    for(int i = DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_INDEX_OFFSET; i < 6+DSANDGRAINS_SCROLLABLE_LIST_SIZE; i++) { \
        button_settings_array[i].context.interactive_list = &g_instances_list_context; \
        button_settings_array[i].index = i - DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_INDEX_OFFSET; \
    }

#define INIT_BACKGROUND \
    init_ui_elements( \
        DSTUDIO_FLAG_NONE, \
        &background, \
        background_texture_id, \
        1,      /* Because there only one gl instance */ \
        NULL,   /* There is no configuration callback */ \
        NULL    /* There is no configuration parameters */ \
    );

#define INIT_SYSTEM_USAGE \
    offsets.x = DSANDGRAINS_RESSOURCE_USAGE_PROMPT_X_POS; \
    offsets.y = DSANDGRAINS_RESSOURCE_USAGE_PROMPT_Y_POS; \
    init_ui_elements( \
        DSTUDIO_FLAG_NONE, \
        &system_usage, \
        system_usage_texture_id, \
        1,          /* Because there is only one gl instance */ \
        NULL,       /* There is no configuration callback */ \
        &offsets \
    ); \
    text_params.scale_matrix = charset_scale_matrix; \
    text_params.gl_x = DSANDGRAINS_CPU_N_MEM_USAGE_X_POS; \
    text_params.gl_y = DSANDGRAINS_CPU_USAGE_Y_POS; \
    text_params.string_size = DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE; \
    init_ui_elements( \
        DSTUDIO_FLAG_NONE, \
        &g_cpu_usage, \
        charset_texture_id, \
        DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE, \
        configure_text_element, \
        &text_params \
    ); \
    text_params.gl_y = DSANDGRAINS_MEM_USAGE_Y_POS; \
    init_ui_elements( \
        DSTUDIO_FLAG_NONE, \
        &g_mem_usage, \
        charset_texture_id, \
        DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE, \
        configure_text_element, \
        &text_params \
    ); \
    init_system_usage_ui(DSANDGRAINS_RESSOURCE_USAGE_STRING_SIZE);   


#define INIT_INSTANCE_SCROLLABLE_LIST \
    text_params.scale_matrix = charset_small_scale_matrix; \
    text_params.gl_x = DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_X_POS; \
    text_params.gl_y = DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_Y_POS; \
    text_params.string_size = DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE; \
    for (int i = 0; i < DSANDGRAINS_SCROLLABLE_LIST_SIZE; i++) { \
        if (i) { \
            text_params.gl_y -= (DSANDGRAINS_SCROLLABLE_LIST_LINE_HEIGHT / ((GLfloat) (DSTUDIO_VIEWPORT_HEIGHT >> 1))); \
        } \
        init_ui_elements( \
            DSTUDIO_FLAG_NONE, \
            &instances[i], \
            charset_small_texture_id, \
            DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE, \
            configure_text_element, \
            &text_params \
        ); \
    } \
    UIInteractiveListSetting instance_interactive_list_settings = { \
        DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SHADOW_X_POS, \
        DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SHADOW_Y_POS, \
        DSANDGRAINS_INSTANCES_MIN_AREA_X, \
        DSANDGRAINS_INSTANCES_MAX_AREA_X, \
        DSANDGRAINS_INSTANCES_MIN_AREA_Y, \
        DSANDGRAINS_INSTANCES_MAX_AREA_Y, \
        DSANDGRAINS_SCROLLABLE_LIST_SHADOW_OFFSET, \
        DSANDGRAINS_SCROLLABLE_LIST_AREA_OFFSET, \
    };\
    params.settings = &instance_interactive_list_settings;\
    init_instances_ui( \
        &instances[0], \
        DSANDGRAINS_SCROLLABLE_LIST_SIZE, \
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE, \
        &params \
    ); \
    params.array_offset += DSANDGRAINS_SCROLLABLE_LIST_SIZE;

#define INIT_VOICE_SCROLLABLE_LIST \
    text_params.gl_y = DSANDGRAINS_VOICE_SCROLLABLE_LIST_Y_POS; \
    for (int i = 0; i < DSANDGRAINS_SCROLLABLE_LIST_SIZE; i++) { \
        if (i) { \
            text_params.gl_y -= (DSANDGRAINS_SCROLLABLE_LIST_LINE_HEIGHT/((GLfloat) (DSTUDIO_VIEWPORT_HEIGHT >> 1))); \
        } \
        init_ui_elements( \
            DSTUDIO_FLAG_NONE, \
            &voices[i], \
            charset_small_texture_id, \
            DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE, \
            configure_text_element, \
            &text_params \
        ); \
    } \
    init_voices_ui( \
        &voices[0], \
        DSANDGRAINS_SCROLLABLE_LIST_SIZE, \
        DSANDGRAINS_SCROLLABLE_LIST_STRING_SIZE \
    ); \
    params.array_offset += DSANDGRAINS_SCROLLABLE_LIST_SIZE;


#define INIT_SCROLLABLE_LIST_ARROWS \
    buttons_settings_array.ui_element_type = DSTUDIO_BUTTON_TYPE_REBOUNCE; \
    buttons_settings_array.gl_x = DSANDGRAINS_INSTANCES_ARROW_UP_X_POS; \
    buttons_settings_array.gl_y = DSANDGRAINS_INSTANCES_ARROW_UP_Y_POS; \
    buttons_settings_array.min_area_x = DSANDGRAINS_INSTANCES_MIN_AREA_X; \
    buttons_settings_array.max_area_x = DSANDGRAINS_INSTANCES_MAX_AREA_X; \
    buttons_settings_array.min_area_y = DSANDGRAINS_INSTANCES_ARROW_UP_MIN_AREA_Y; \
    buttons_settings_array.max_area_y = DSANDGRAINS_INSTANCES_ARROW_UP_MAX_AREA_Y; \
    params.update_callback = update_button; \
    params.settings = &buttons_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_NONE, \
        &arrow_instances_top, \
        button_settings_array[0].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = DSANDGRAINS_INSTANCES_ARROW_DOWN_Y_POS; \
    buttons_settings_array.min_area_y = DSANDGRAINS_INSTANCES_ARROW_DOWN_MIN_AREA_Y; \
    buttons_settings_array.max_area_y = DSANDGRAINS_INSTANCES_ARROW_DOWN_MAX_AREA_Y; \
    init_ui_elements( \
        DSTUDIO_FLAG_FLIP_Y, \
        &arrow_instances_bottom, \
        button_settings_array[1].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = DSANDGRAINS_VOICES_ARROW_UP_Y_POS; \
    buttons_settings_array.min_area_y = DSANDGRAINS_VOICES_ARROW_UP_MIN_AREA_Y; \
    buttons_settings_array.max_area_y = DSANDGRAINS_VOICES_ARROW_UP_MAX_AREA_Y; \
    init_ui_elements( \
        0, \
        &arrow_voices_top, \
        button_settings_array[2].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = DSANDGRAINS_VOICES_ARROW_DOWN_Y_POS; \
    buttons_settings_array.min_area_y = DSANDGRAINS_VOICES_ARROW_DOWN_MIN_AREA_Y; \
    buttons_settings_array.max_area_y = DSANDGRAINS_VOICES_ARROW_DOWN_MAX_AREA_Y; \
    init_ui_elements( \
        DSTUDIO_FLAG_FLIP_Y, \
        &arrow_voices_bottom, \
        button_settings_array[3].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = DSANDGRAINS_SAMPLES_ARROW_UP_Y_POS; \
    buttons_settings_array.min_area_y = DSANDGRAINS_SAMPLES_ARROW_UP_MIN_AREA_Y; \
    buttons_settings_array.max_area_y = DSANDGRAINS_SAMPLES_ARROW_UP_MAX_AREA_Y; \
    init_ui_elements( \
        0, \
        &arrow_samples_top, \
        button_settings_array[4].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = DSANDGRAINS_SAMPLES_ARROW_DOWN_Y_POS; \
    buttons_settings_array.min_area_y = DSANDGRAINS_SAMPLES_ARROW_DOWN_MIN_AREA_Y; \
    buttons_settings_array.max_area_y = DSANDGRAINS_SAMPLES_ARROW_DOWN_MAX_AREA_Y; \
    init_ui_elements( \
        DSTUDIO_FLAG_FLIP_Y, \
        &arrow_samples_bottom, \
        button_settings_array[5].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1;

#define INIT_SAMPLE_KNOBS \
    init_ui_elements_settings( \
        &knobs_settings_array, \
        DSANDGRAINS_SAMPLE_KNOBS_POS_X, \
        DSANDGRAINS_SAMPLE_KNOBS_POS_Y, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X, \
        DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y, \
        DSANDGRAINS_SAMPLE_KNOBS_ROWS, \
        DSANDGRAINS_SAMPLE_KNOBS_COUNT, \
        DSTUDIO_KNOB_TYPE_CONTINUE \
    ); \
    params.update_callback = update_ui_element_motion; \
    params.settings = knobs_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &sample_knobs, \
        knob1_texture_id, \
        DSANDGRAINS_SAMPLE_KNOBS_COUNT, \
        configure_ui_element, \
        &params \
    ); \
    free(knobs_settings_array); \
    params.array_offset += DSANDGRAINS_SAMPLE_KNOBS_COUNT;

#define INIT_SAMPLE_LFO_KNOBS \
    init_ui_elements_settings( \
        &knobs_settings_array, \
        DSANDGRAINS_LFO_KNOBS_POS_X, \
        DSANDGRAINS_LFO_KNOBS_POS_Y, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_LFO_KNOBS_OFFSET_X, \
        DSANDGRAINS_LFO_KNOBS_OFFSET_Y, \
        DSANDGRAINS_LFO_KNOBS_ROWS, \
        DSANDGRAINS_SAMPLE_LFO_KNOBS_COUNT, \
        DSTUDIO_KNOB_TYPE_CONTINUE \
    ); \
    params.settings = knobs_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &sample_lfo_knobs, \
        knob2_texture_id, \
        DSANDGRAINS_SAMPLE_LFO_KNOBS_COUNT, \
        configure_ui_element, \
        &params \
    ); \
    free(knobs_settings_array); \
    params.array_offset += DSANDGRAINS_SAMPLE_LFO_KNOBS_COUNT;
    
#define INIT_SAMPLE_LFO_PITCH_KNOBS \
    init_ui_elements_settings( \
        &knobs_settings_array, \
        DSANDGRAINS_LFO_PITCH_KNOBS_POS_X, \
        DSANDGRAINS_LFO_PITCH_KNOBS_POS_Y, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_X, \
        DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_Y, \
        DSANDGRAINS_LFO_PITCH_KNOBS_ROWS, \
        DSANDGRAINS_SAMPLE_LFO_PITCH_KNOBS_COUNT, \
        DSTUDIO_KNOB_TYPE_CONTINUE \
    ); \
    params.settings = knobs_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &sample_lfo_pitch_knobs, \
        knob2_texture_id, \
        DSANDGRAINS_SAMPLE_LFO_PITCH_KNOBS_COUNT, \
        configure_ui_element, \
        &params \
    ); \
    free(knobs_settings_array); \
    params.array_offset += DSANDGRAINS_SAMPLE_LFO_PITCH_KNOBS_COUNT;

#define INIT_AMOUNT_PITCH_KNOBS \
    init_ui_elements_settings( \
        &knobs_settings_array, \
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_X, \
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_Y, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_X, \
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_Y, \
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_ROWS, \
        DSANDGRAINS_SAMPLE_AMOUNT_PITCH_KNOBS_COUNT, \
        DSTUDIO_KNOB_TYPE_CONTINUE \
    );\
    params.settings = knobs_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &sample_amount_pitch_knobs, \
        knob1_texture_id, \
        DSANDGRAINS_SAMPLE_AMOUNT_PITCH_KNOBS_COUNT, \
        configure_ui_element, \
        &params \
    );\
    free(knobs_settings_array); \
    params.array_offset += DSANDGRAINS_SAMPLE_AMOUNT_PITCH_KNOBS_COUNT;

#define INIT_VOICE_KNOBS \
    init_ui_elements_settings( \
        &knobs_settings_array, \
        DSANDGRAINS_VOICE_KNOBS_POS_X, \
        DSANDGRAINS_VOICE_KNOBS_POS_Y, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_VOICE_KNOBS_OFFSET_X, \
        DSANDGRAINS_VOICE_KNOBS_OFFSET_Y, \
        DSANDGRAINS_VOICE_KNOBS_ROWS, \
        DSANDGRAINS_VOICE_KNOBS_COUNT, \
        DSTUDIO_KNOB_TYPE_CONTINUE \
    );\
    params.settings = knobs_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &voice_knobs, \
        knob1_texture_id, \
        DSANDGRAINS_VOICE_KNOBS_COUNT, \
        configure_ui_element, \
        &params \
    );\
    free(knobs_settings_array); \
    params.array_offset += DSANDGRAINS_VOICE_KNOBS_COUNT;

#define INIT_SLIDERS \
    params.update_callback = update_ui_element_motion; \
    init_ui_elements_settings( \
        &sliders_settings_array, \
        DSANDGRAINS_DAHDDSR_POS_X, \
        DSANDGRAINS_DAHDDSR_POS_Y, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDERS_SLIDE, \
        DSANDGRAINS_SLIDERS_OFFSET, \
        0, \
        DSANDGRAINS_DAHDSR_SLIDERS_COUNT, \
        DSANDGRAINS_DAHDSR_SLIDERS_COUNT, \
        DSTUDIO_SLIDER_TYPE_VERTICAL \
    );\
    params.settings = sliders_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &sliders_dahdsr, \
        slider_texture_id, \
        DSANDGRAINS_DAHDSR_SLIDERS_COUNT, \
        configure_ui_element, \
        &params \
    ); \
    free(sliders_settings_array); \
    params.array_offset += DSANDGRAINS_DAHDSR_SLIDERS_COUNT; \
    init_ui_elements_settings( \
        &sliders_settings_array, \
        DSANDGRAINS_EQUALIZER_POS_X, \
        DSANDGRAINS_EQUALIZER_POS_Y, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDERS_SLIDE, \
        DSANDGRAINS_SLIDERS_OFFSET, \
        0, \
        DSANDGRAINS_EQUALIZER_SLIDERS_COUNT, \
        DSANDGRAINS_EQUALIZER_SLIDERS_COUNT, \
        DSTUDIO_SLIDER_TYPE_VERTICAL \
    );\
    params.settings = sliders_settings_array; \
    init_ui_elements( \
        DSTUDIO_FLAG_ANIMATED, \
        &sliders_equalizer, \
        slider_texture_id, \
        DSANDGRAINS_EQUALIZER_SLIDERS_COUNT, \
        configure_ui_element, \
        &params \
    ); \
    free(sliders_settings_array);
