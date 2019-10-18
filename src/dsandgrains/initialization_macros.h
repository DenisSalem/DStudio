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

#define LOAD_SHARED_TEXTURE_AND_PREPARE_SHARED_SCALE_MATRICES \
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
    ); \
    DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX( \
        charset_scale_matrix, \
        DSANDGRAINS_CHARSET_WIDTH, \
        DSANDGRAINS_CHARSET_HEIGHT \
    ) \
    DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX( \
        charset_small_scale_matrix, \
        DSANDGRAINS_CHARSET_SMALL_WIDTH, \
        DSANDGRAINS_CHARSET_SMALL_HEIGHT \
    )

#define SETUP_BUTTONS_SETTING_ARRAYS \
    button_settings_array[0].release = setup_texture_n_scale_matrix(DSTUDIO_FLAG_USE_ALPHA, 117, 8, DSTUDIO_ARROW_INSTANCES_ASSET_PATH, arrow_instances_scale_matrix); \
    button_settings_array[0].active = setup_texture_n_scale_matrix(DSTUDIO_FLAG_USE_ALPHA, 117, 8, DSTUDIO_ACTIVE_ARROW_INSTANCES_ASSET_PATH, NULL); \
    button_settings_array[0].type = DSTUDIO_BUTTON_TYPE_1; \
    button_settings_array[0].application_callback = scroll_instances; \
    button_settings_array[0].flags = DSTUDIO_BUTTON_ACTION_LIST_BACKWARD; \
    button_settings_array[1].release = button_settings_array[0].release; \
    button_settings_array[1].active = button_settings_array[0].active; \
    button_settings_array[1].type = DSTUDIO_BUTTON_TYPE_1; \
    button_settings_array[1].application_callback = scroll_instances; \
    button_settings_array[2].release = button_settings_array[0].release; \
    button_settings_array[2].active = button_settings_array[0].active; \
    button_settings_array[2].type = DSTUDIO_BUTTON_TYPE_1; \
    button_settings_array[2].application_callback = scroll_voices; \
    button_settings_array[2].flags = DSTUDIO_BUTTON_ACTION_LIST_BACKWARD; \
    button_settings_array[3].release = button_settings_array[0].release; \
    button_settings_array[3].active = button_settings_array[0].active; \
    button_settings_array[3].type = DSTUDIO_BUTTON_TYPE_1; \
    button_settings_array[3].application_callback = scroll_voices; \
    button_settings_array[4].release = button_settings_array[0].release; \
    button_settings_array[4].active = button_settings_array[0].active; \
    button_settings_array[4].type = DSTUDIO_BUTTON_TYPE_1; \
    button_settings_array[4].application_callback = scroll_voices; \
    button_settings_array[4].flags = DSTUDIO_BUTTON_ACTION_LIST_BACKWARD; \
    button_settings_array[5].release = button_settings_array[0].release; \
    button_settings_array[5].active = button_settings_array[0].active; \
    button_settings_array[5].type = DSTUDIO_BUTTON_TYPE_1; \
    button_settings_array[5].application_callback = scroll_voices;

#define SETUP_KNOBS_SETTING_ARRAYS \
    UIElementSetting sample_knobs_settings_array[DSANDGRAINS_SAMPLE_KNOBS] = { \
        {-0.8675, 0.25, 20.0,  85.0,  147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: START */ \
        {-0.7075, 0.25, 84.0,  149.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: END */ \
        {-0.5475, 0.25, 148.0, 213.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: GRAIN SIZE */ \
        {-0.3875, 0.25, 212.0, 277.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: TRANSIENT THRESOLD */ \
        {-0.8675, -.15, 20.0,  85.0,  243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LOOP DURATION */ \
        {-0.7075, -.15, 84.0,  149.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: CLOUD SIZE */ \
        {-0.5475, -.15, 148.0, 213.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: DISTRIBUTION BALANCE */ \
        {-0.3875, -.15, 212.0, 277.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: SMOOTH TRANSITION */ \
    }; \
    UIElementSetting sample_small_knobs_settings_array[DSANDGRAINS_SAMPLE_SMALL_KNOBS] = { \
        {-0.222499,  0.25, 286.0, 335.0, 155.0, 204.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LFO MODULATION TUNE */ \
        {-0.102500,  0.25, 334.0, 383.0, 155.0, 204.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LFO MODULATION PHASE */ \
        {-0.222499,  0.004166,    286.0, 335.0, 214.0, 262.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LFO MODULATION DEPTH */ \
        {-0.102500,  0.004166,    334.0, 383.0, 214.0, 262.0, DSTUDIO_KNOB_TYPE_2}, /* SAMPLE: LFO MODULATION SIGNAL */ \
        {-0.2225,   -0.5125,      286.0, 335.0, 338.0, 387.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LFO PITCH MOD TUNE */ \
        {-0.1025,   -0.5125,      334.0, 383.0, 338.0, 387.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LFO PITCH MOD PHASE */ \
        {-0.2225,   -0.758333,    286.0, 335.0, 397.0, 446.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: LFO PITCH MOD DEPTH */ \
        {-0.1025,   -0.758333,    334.0, 383.0, 397.0, 446.0, DSTUDIO_KNOB_TYPE_2}, /* SAMPLE: LFO PITCH MOD SIGNAL */ \
        { 0.0375,    0.3,         390.0, 439.0, 143.0, 192.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: AMOUNT */ \
        { 0.1675,    0.3,         442.0, 491.0, 143.0, 192.0, DSTUDIO_KNOB_TYPE_1}, /* SAMPLE: PITCH */ \
    }; \
    UIElementSetting voice_knobs_settings_array[DSANDGRAINS_VOICE_KNOBS] = { \
        { 0.3725,  0.270833, 516,   581.0, 142.0, 207.0, DSTUDIO_KNOB_TYPE_1}, /* VOICE : VOLUME */ \
        { 0.5475,  0.270833, 586.0, 651.0, 142.0, 207.0, DSTUDIO_KNOB_TYPE_1}, /* VOICE : PAN */ \
        { 0.5475, -0.070833, 586.0, 651.0, 224,   289.0, DSTUDIO_KNOB_TYPE_1}, /* VOICE : DENSITY */ \
        { 0.3725, -0.070833, 516,   581.0, 224,   289.0, DSTUDIO_KNOB_TYPE_1}  /* VOICE : INFLUENCE */ \
    };

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
    );

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
    }

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
    );

#define INIT_SCROLLABLE_LIST_ARROWS \
    buttons_settings_array.gl_x = 0.81875; \
    buttons_settings_array.gl_y = 0.404166; \
    buttons_settings_array.min_area_x = 669.0; \
    buttons_settings_array.max_area_x = 787.0; \
    buttons_settings_array.min_area_y = 139.0; \
    buttons_settings_array.max_area_y = 148.0; \
    buttons_settings_array.ui_element_type = DSTUDIO_BUTTON_TYPE_1; \
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
    buttons_settings_array.gl_y = 0.04166; \
    buttons_settings_array.min_area_y = 226; \
    buttons_settings_array.max_area_y = 235; \
    init_ui_elements( \
        DSTUDIO_FLAG_FLIP_Y, \
        &arrow_instances_bottom, \
        button_settings_array[1].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = -0.058333; \
    buttons_settings_array.min_area_y = 250; \
    buttons_settings_array.max_area_y = 259; \
    init_ui_elements( \
        0, \
        &arrow_voices_top, \
        button_settings_array[2].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = -0.420833; \
    buttons_settings_array.min_area_y = 337; \
    buttons_settings_array.max_area_y = 346; \
    init_ui_elements( \
        DSTUDIO_FLAG_FLIP_Y, \
        &arrow_voices_bottom, \
        button_settings_array[3].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = -0.520833; \
    buttons_settings_array.min_area_y = 361; \
    buttons_settings_array.max_area_y = 370; \
    init_ui_elements( \
        0, \
        &arrow_samples_top, \
        button_settings_array[4].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1; \
    buttons_settings_array.gl_y = -0.883333; \
    buttons_settings_array.min_area_y = 448; \
    buttons_settings_array.max_area_y = 457; \
    init_ui_elements( \
        DSTUDIO_FLAG_FLIP_Y, \
        &arrow_samples_bottom, \
        button_settings_array[5].release, \
        1, \
        configure_ui_element, \
        &params \
    ); \
    params.array_offset +=1;
