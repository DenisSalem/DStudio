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

#include "../extensions.h"

#include "../window_management.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../buttons.h"
#include "../common.h"
#include "../fileutils.h"
#include "../instances.h"
#include "../knobs.h"
#include "../text.h"
#include "instances.h"
#include "ui.h"

static UISystemUsage * ui_system_usage_p;
static UIInstances * ui_instances_p;

/* Background */
static UIElements background = {0};
static Vec2 background_scale_matrix[2] = {0};

/* Charsets */
static Vec2 charset_scale_matrix[2] = {0};
static Vec2 charset_small_scale_matrix[2] = {0};

/* Buttons management */
ButtonsManagement buttons_management = {0};

/* Arrows */
static UIElements arrow_instances_top = {0};
static UIElements arrow_instances_bottom = {0};
static UIElements arrow_voices_top = {0};
static UIElements arrow_voices_bottom = {0};
static UIElements arrow_samples_top = {0};
static UIElements arrow_samples_bottom = {0};

static Vec2 arrow_instances_scale_matrix[2] = {0};

/* Instances */
static UIElements instances[7] = {0};

/* System Usage */
static UIElements system_usage = {0};
static UIElements cpu_usage = {0};
static UIElements mem_usage = {0};
static Vec2 system_usage_scale_matrix[2] = {0};

/* Knobs 1 */
static UIElements sample_knobs = {0};
static UIElements voice_knobs = {0};
static Vec2 knob1_scale_matrix[2] = {0};

/* Knobs 2 */
static UIElements sample_small_knobs = {0};
static Vec2 knob2_scale_matrix[2] = {0};

/* Sliders */
static UIElements sliders_dahdsr = {0};
//~ static UIElements sliders_dahdsr_pitch = {0};
//~ static UIElements sliders_dahdsr_lfo = {0};
//~ static UIElements sliders_dahdsr_lfo_pitch = {0};
static UIElements sliders_equalizer = {0};
static Vec2 slider_scale_matrix[2] = {0};

UIArea ui_areas[DSANDGRAINS_UI_ELEMENTS_COUNT] = {0};
UICallback ui_callbacks[DSANDGRAINS_UI_ELEMENTS_COUNT] = {0};

static useconds_t framerate = 20000;
static char areas_index = -1;

static GLuint interactive_program_id, non_interactive_program_id;
static GLuint interactive_scale_matrix_id, non_interactive_scale_matrix_id;
static GLuint motion_type_id;

static GLfloat motion_type;

static ButtonStates button_states_array[DSANDGRAINS_BUTTONS_COUNT] = {0};

#include "../ui_statics.h"

static void init_ui(UI * ui) {
    /* Will hold every sliders settings */
    UIElementSetting * sliders_settings_array = 0;
    
    /* Setup shared memory */
    ui_system_usage_p = &ui->system_usage;
    ui_instances_p = &ui->instances;
    
    /* Load shared texture and prepare shared scale matrices */
    GLuint background_texture_id = setup_texture_n_scale_matrix(0, 0, 800, 480, DSANDGRAINS_BACKGROUND_ASSET_PATH, background_scale_matrix);
    GLuint knob1_texture_id = setup_texture_n_scale_matrix(1, 1, 64, 64, DSANDGRAINS_KNOB1_ASSET_PATH, knob1_scale_matrix);
    GLuint knob2_texture_id = setup_texture_n_scale_matrix(1, 1, 48, 48, DSANDGRAINS_KNOB2_ASSET_PATH, knob2_scale_matrix);
    GLuint slider_texture_id = setup_texture_n_scale_matrix(0, 1, 10, 10, DSANDGRAINS_SLIDER1_ASSET_PATH, slider_scale_matrix);
    GLuint system_usage_texture_id = setup_texture_n_scale_matrix(0, 1, 30, 23, DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH, system_usage_scale_matrix);
    GLuint charset_texture_id = setup_texture_n_scale_matrix(0, 1, 104, 234, DSTUDIO_CHAR_TABLE_ASSET_PATH, NULL);
    DSTUDIO_SET_TEXT_SCALE_MATRIX(charset_scale_matrix, 104, 234)
    GLuint charset_small_texture_id = setup_texture_n_scale_matrix(0, 1, 52, 117, DSTUDIO_CHAR_TABLE_SMALL_ASSET_PATH, NULL);
    DSTUDIO_SET_TEXT_SCALE_MATRIX(charset_small_scale_matrix, 52, 117)
    
    DSTUDIO_EXIT_IF_FAILURE(pthread_create( &buttons_management.thread_id, NULL, buttons_management_thread, &buttons_management))

    /* Setting up button states */
    button_states_array[0].release = setup_texture_n_scale_matrix(0, 1, 117, 8, DSTUDIO_ARROW_INSTANCES_ASSET_PATH, arrow_instances_scale_matrix);
    button_states_array[0].active = setup_texture_n_scale_matrix(0, 1, 117, 8, DSTUDIO_ACTIVE_ARROW_INSTANCES_ASSET_PATH, NULL);
    button_states_array[0].type = DSTUDIO_BUTTON_TYPE_1;
    
    button_states_array[1].release = button_states_array[0].release;
    button_states_array[1].active = button_states_array[0].active;
    button_states_array[1].type = DSTUDIO_BUTTON_TYPE_1;

    button_states_array[2].release = button_states_array[0].release;
    button_states_array[2].active = button_states_array[0].active;
    button_states_array[2].type = DSTUDIO_BUTTON_TYPE_1;
    
    button_states_array[3].release = button_states_array[0].release;
    button_states_array[3].active = button_states_array[0].active;
    button_states_array[3].type = DSTUDIO_BUTTON_TYPE_1;

    button_states_array[4].release = button_states_array[0].release;
    button_states_array[4].active = button_states_array[0].active;
    button_states_array[4].type = DSTUDIO_BUTTON_TYPE_1;

    button_states_array[5].release = button_states_array[0].release;
    button_states_array[5].active = button_states_array[0].active;
    button_states_array[5].type = DSTUDIO_BUTTON_TYPE_1;
    
    /* - Tell to mouse button callback the height of the current active slider.
     * - Help to init slider settings.*/
    slider_texture_scale = 10;
    
    UIElementSetting sample_knobs_settings_array[DSANDGRAINS_SAMPLE_KNOBS] = {
        {-0.8675, 0.25, 20.0,  85.0,  147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: START
        {-0.7075, 0.25, 84.0,  149.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: END
        {-0.5475, 0.25, 148.0, 213.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: GRAIN SIZE
        {-0.3875, 0.25, 212.0, 277.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: TRANSIENT THRESOLD
        {-0.8675, -.15, 20.0,  85.0,  243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LOOP DURATION
        {-0.7075, -.15, 84.0,  149.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: CLOUD SIZE
        {-0.5475, -.15, 148.0, 213.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: DISTRIBUTION BALANCE
        {-0.3875, -.15, 212.0, 277.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: SMOOTH TRANSITION
    };
    
    UIElementSetting sample_small_knobs_settings_array[DSANDGRAINS_SAMPLE_SMALL_KNOBS] = {
        {-0.222499,  0.25, 286.0, 335.0, 155.0, 204.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION TUNE
        {-0.102500,  0.25, 334.0, 383.0, 155.0, 204.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION PHASE
        {-0.222499,  0.004166,    286.0, 335.0, 214.0, 262.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION DEPTH
        {-0.102500,  0.004166,    334.0, 383.0, 214.0, 262.0, DSTUDIO_KNOB_TYPE_2}, // SAMPLE: LFO MODULATION SIGNAL
        {-0.2225,   -0.5125,      286.0, 335.0, 338.0, 387.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD TUNE
        {-0.1025,   -0.5125,      334.0, 383.0, 338.0, 387.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD PHASE
        {-0.2225,   -0.758333,    286.0, 335.0, 397.0, 446.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD DEPTH
        {-0.1025,   -0.758333,    334.0, 383.0, 397.0, 446.0, DSTUDIO_KNOB_TYPE_2}, // SAMPLE: LFO PITCH MOD SIGNAL
        { 0.0375,    0.3,         390.0, 439.0, 143.0, 192.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: AMOUNT
        { 0, 0.0, 440.0, 488.0, 409.0, 458.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: PITCH
    };
    
    UIElementSetting voice_knobs_settings_array[DSANDGRAINS_VOICE_KNOBS] = {
        { 0.5475,  0.270833, 586.0, 651.0, 142.0, 207.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : DENSITY
        { 0.3725, -0.129166, 516,   581.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : VOLUME
        { 0.5475, -0.129166, 586.0, 651.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : PAN
    };
    
    /* Background */
    init_ui_elements(0, &background, background_texture_id, 1, NULL, NULL);

    /* System Usage */
    Vec4 offsets = {-0.035, 0.889583, 0, 0};
    init_ui_elements(0, &system_usage, system_usage_texture_id, 1, NULL, &offsets);

    /* CPU & MEM Usage */
    UITextSettingParams text_params;
    text_params.scale_matrix = charset_scale_matrix;   
    
    text_params.gl_x = -0.035 + ((GLfloat) (30+10) / ((GLfloat) DSTUDIO_VIEWPORT_WIDTH));
    text_params.gl_y = 0.916666;
    text_params.string_size = 6;
    init_ui_elements(0, &cpu_usage, charset_texture_id, 6, configure_text_element, &text_params);

    text_params.gl_x = -0.035 + ((GLfloat) (30+10) / ((GLfloat) DSTUDIO_VIEWPORT_WIDTH));
    text_params.gl_y = 0.862499;
    text_params.string_size = 6;
    init_ui_elements(0, &mem_usage, charset_texture_id, 6, configure_text_element, &text_params);
    
    /* Instances */
    text_params.scale_matrix = charset_small_scale_matrix;   
    text_params.gl_x = 0.678;
    text_params.string_size = 29;
        
    for (int i = 0; i < 7; i++) {
        text_params.gl_y = 0.360416 - i * (11.0/((GLfloat) (DSTUDIO_VIEWPORT_HEIGHT >> 1)));
        init_ui_elements(0, &instances[i], charset_small_texture_id, 29, configure_text_element, &text_params);
    }
    
    init_instances_ui(&instances[0], 7, 29);
    
    /* Inits interactive ui elements */
    UIElementSettingParams params = {0};
    params.callbacks = ui_callbacks;
    params.areas = ui_areas;

    /* Instance arrows */
    UIElementSetting buttons_settings_array = {0.81875, 0.404166, 669.0,  787.0,  139.0, 148.0, DSTUDIO_BUTTON_TYPE_1};
    params.update_callback = update_button;
    params.settings = &buttons_settings_array;

    init_ui_elements(0, &arrow_instances_top, button_states_array[0].release, 1, configure_ui_element, &params);
    params.array_offset +=1;

    buttons_settings_array.gl_y = 0.04166;
    buttons_settings_array.min_area_y = 226;
    buttons_settings_array.max_area_y = 235;
    init_ui_elements(DSTUDIO_FLAG_FLIP_Y, &arrow_instances_bottom, button_states_array[1].release, 1, configure_ui_element, &params);
    params.array_offset +=1;
    
    buttons_settings_array.gl_y = -0.058333;
    buttons_settings_array.min_area_y = 250;
    buttons_settings_array.max_area_y = 259;
    init_ui_elements(0, &arrow_voices_top, button_states_array[2].release, 1, configure_ui_element, &params);
    params.array_offset +=1;
    
    buttons_settings_array.gl_y = -0.420833;
    buttons_settings_array.min_area_y = 337;
    buttons_settings_array.max_area_y = 346;
    init_ui_elements(DSTUDIO_FLAG_FLIP_Y, &arrow_voices_bottom, button_states_array[3].release, 1, configure_ui_element, &params);
    params.array_offset +=1;

    buttons_settings_array.gl_y = -0.520833;
    buttons_settings_array.min_area_y = 361;
    buttons_settings_array.max_area_y = 370;
    init_ui_elements(0, &arrow_samples_top, button_states_array[4].release, 1, configure_ui_element, &params);
    params.array_offset +=1;

    buttons_settings_array.gl_y = -0.883333;
    buttons_settings_array.min_area_y = 448;
    buttons_settings_array.max_area_y = 457;
    init_ui_elements(DSTUDIO_FLAG_FLIP_Y, &arrow_samples_bottom, button_states_array[5].release, 1, configure_ui_element, &params);
    params.array_offset +=1;
    
    /* Knobs */
    params.update_callback = update_knob;
    params.settings = sample_knobs_settings_array;
    init_ui_elements(DSTUDIO_FLAG_ANIMATED, &sample_knobs, knob1_texture_id, 8, configure_ui_element, &params);
    
    params.settings = sample_small_knobs_settings_array;
    params.array_offset += 8;
    init_ui_elements(DSTUDIO_FLAG_ANIMATED, &sample_small_knobs, knob2_texture_id, 10, configure_ui_element, &params);
    
    params.settings = voice_knobs_settings_array;
    params.array_offset += 10;
    init_ui_elements(DSTUDIO_FLAG_ANIMATED, &voice_knobs, knob1_texture_id, 3, configure_ui_element, &params);
    
    /* Sliders */
    params.update_callback = update_slider;
    
    params.array_offset += 3;
    init_slider_settings(&sliders_settings_array, slider_texture_scale, 339, 441, 16, 16, DSANDGRAINS_DAHDSR_SLIDERS_COUNT);
    params.settings = sliders_settings_array;
    init_ui_elements(DSTUDIO_FLAG_ANIMATED, &sliders_dahdsr, slider_texture_id, DSANDGRAINS_DAHDSR_SLIDERS_COUNT, configure_ui_element, &params);
    free(sliders_settings_array);

    params.array_offset += DSANDGRAINS_DAHDSR_SLIDERS_COUNT;
    init_slider_settings(&sliders_settings_array, slider_texture_scale, 523, 359, 16, 16, DSANDGRAINS_EQUALIZER_SLIDERS_COUNT);
    params.settings = sliders_settings_array;
    init_ui_elements(DSTUDIO_FLAG_ANIMATED, &sliders_equalizer, slider_texture_id, DSANDGRAINS_EQUALIZER_SLIDERS_COUNT, configure_ui_element, &params);
    free(sliders_settings_array);

    init_system_usage_ui(ui_system_usage_p, 6);   
    init_buttons_management(&buttons_management, &button_states_array[0], DSANDGRAINS_BUTTONS_COUNT);

    /* Setting shader uniform input ID */
    non_interactive_scale_matrix_id = glGetUniformLocation(non_interactive_program_id, "scale_matrix");
    interactive_scale_matrix_id = glGetUniformLocation(interactive_program_id, "scale_matrix");
    motion_type_id = glGetUniformLocation(interactive_program_id, "motion_type");
}

static void render_viewport(int mask) {
    glUseProgram(non_interactive_program_id);
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) background_scale_matrix);
        render_ui_elements(&background);
        
        // SYSTEM USAGE
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) system_usage_scale_matrix);
        render_ui_elements(&system_usage);
        
        // CPU & MEM USAGE
        if (mask & DSTUDIO_RENDER_SYSTEM_USAGE) {
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) charset_scale_matrix);
            render_ui_elements(&cpu_usage);
            render_ui_elements(&mem_usage);
        }
        
        // ARROWS
        if (mask & DSTUDIO_RENDER_BUTTONS_TYPE_1) {
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) arrow_instances_scale_matrix);
            render_ui_elements(&arrow_instances_bottom);
            render_ui_elements(&arrow_instances_top);
            render_ui_elements(&arrow_voices_top);
            render_ui_elements(&arrow_voices_bottom);
            render_ui_elements(&arrow_samples_top);
            render_ui_elements(&arrow_samples_bottom);
        }
        
        // INSTANCES
        if (mask & DSTUDIO_RENDER_INSTANCES) {
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) charset_small_scale_matrix);
            for (unsigned int i = 0; i < ui_instances_p->lines_number; i++) {
                render_ui_elements(&ui_instances_p->lines[i]);
            }
        }


    glUseProgram(interactive_program_id);
        // KNOBS
        if (mask & DSTUDIO_RENDER_KNOBS) {
            motion_type = 0.0;
            glUniform1f(motion_type_id, motion_type);
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) knob1_scale_matrix);
            render_ui_elements(&sample_knobs);

            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) knob2_scale_matrix);
            render_ui_elements(&sample_small_knobs);
        
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) knob1_scale_matrix);
            render_ui_elements(&voice_knobs);
        }

        // SLIDERS
        if (mask & DSTUDIO_RENDER_SLIDERS) {
            motion_type = 1.0;
            glUniform1f(motion_type_id, motion_type);
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) slider_scale_matrix);
            render_ui_elements(&sliders_dahdsr);
            render_ui_elements(&sliders_equalizer);
        }
}

// Should be splitted
void * ui_thread(void * arg) {
    //int redraw_all = 0;

    init_context("DSANDGRAINS", DSTUDIO_VIEWPORT_WIDTH, DSTUDIO_VIEWPORT_HEIGHT);
    set_mouse_button_callback(mouse_button_callback);
    set_cursor_position_callback(cursor_position_callback);
    	
    DSTUDIO_EXIT_IF_FAILURE(load_extensions())
    
    create_shader_program(&interactive_program_id, &non_interactive_program_id);
    
    init_ui( (UI *) arg);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    
    while (do_no_exit_loop()) {
        usleep(framerate);
        
        /* RENDER */

        if (need_to_redraw_all()) {
            glScissor(0, 0, DSTUDIO_VIEWPORT_WIDTH, DSTUDIO_VIEWPORT_HEIGHT);
            render_viewport(DSTUDIO_RENDER_ALL);
        }
        else {
            check_for_buttons_to_render_n_update(
                &buttons_management,
                render_viewport,
                ui_callbacks,
                mouse_state,
                ui_areas
            );

            // Check for knob or slider to render
            if (areas_index >= 0) {
                glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
                render_viewport(render_mask);
            }
            
            //~ // UPDATE AND RENDER TEXT
            sem_wait(&ui_system_usage_p->mutex);
            if (ui_system_usage_p->update /*&& !redraw_all*/) {
                update_text(&cpu_usage, ui_system_usage_p->cpu_string_buffer, ui_system_usage_p->string_size);
                update_text(&mem_usage, ui_system_usage_p->mem_string_buffer, ui_system_usage_p->string_size);
                glScissor(402, 438, 48, 31);
                render_viewport(DSTUDIO_RENDER_SYSTEM_USAGE);
                ui_system_usage_p->update = 0;
            }
            sem_post(&ui_system_usage_p->mutex);

            sem_wait(&ui_instances_p->mutex);
            if (ui_instances_p->update /*&& !redraw_all*/) {
                update_instances_text();
                glScissor(669, 254, 117, 79);
                render_viewport(DSTUDIO_RENDER_INSTANCES);
                ui_instances_p->update = 0;
            }
            sem_post(&ui_instances_p->mutex);
        }
                
        swap_window_buffer();
        listen_events();
    }
    
    sem_wait(&ui_system_usage_p->mutex);
    ui_system_usage_p->cut_thread = 1;
    sem_post(&ui_system_usage_p->mutex);

    sem_wait(&buttons_management.mutex);
    buttons_management.cut_thread = 1;
    sem_post(&buttons_management.mutex);
    DSTUDIO_EXIT_IF_FAILURE(pthread_join(buttons_management.thread_id, NULL))

    exit_instances_thread();
    destroy_context();
    return NULL;
}
