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

/* System Usage */
static UIElements system_usage = {0};
static UIElements cpu_usage = {0};
static Vec2 system_usage_scale_matrix[2] = {0};
static Vec2 charset_scale_matrix[2] = {0};

/* Knobs 1 */
static UIElements sample_knobs = {0};
static UIElements voice_knobs = {0};
static Vec2 knob1_scale_matrix[2] = {0};

/* Knobs 2 */
static UIElements sample_small_knobs = {0};
static Vec2 knob2_scale_matrix[2] = {0};

/* Sliders */
static UIElements sliders_dahdsr = {0};
static UIElements sliders_dahdsr_pitch = {0};
static UIElements sliders_dahdsr_lfo = {0};
static UIElements sliders_dahdsr_lfo_pitch = {0};
static UIElements sliders_equalizer = {0};
static Vec2 slider_scale_matrix[2] = {0};

UIArea ui_areas[DSANDGRAINS_UI_ELEMENTS_COUNT] = {0};
UICallback ui_callbacks[DSANDGRAINS_UI_ELEMENTS_COUNT] = {0};

static useconds_t framerate = 20000;
static char first_render = 1;
static char areas_index = -1;

static GLint scissor_x, scissor_y;
static GLsizei scissor_width, scissor_height;

static GLuint interactive_program_id, non_interactive_program_id;
static GLuint interactive_scale_matrix_id, non_interactive_scale_matrix_id, background_element_offset_id;
static GLuint motion_type_id;

static const GLfloat * ui_system_usage_scale_matrix_p;
static const GLfloat * ui_text_system_usage_scale_matrix_p;
static const GLfloat * sample_knobs_scale_matrix_p;
static const GLfloat * sample_small_knobs_scale_matrix_p;
static const GLfloat * sliders_dahdsr_scale_matrix_p;
static const GLfloat * sliders_dahdsr_pitch_scale_matrix_p;
static const GLfloat * sliders_dahdsr_lfo_scale_matrix_p;
static const GLfloat * sliders_dahdsr_lfo_pitch_scale_matrix_p;
static const GLfloat * sliders_equalizer_scale_matrix_p;
static const GLfloat * voice_knobs_scale_matrix_p;
static GLfloat motion_type;

#include "../ui_statics.h"

static void init_ui(UI * ui) {
    /* Setup shared memory */
    ui_system_usage_p = &ui->system_usage;
    ui_instances_p = &ui->instances;
    
    /* Load shared texture and prepare shared scale_matrix */
    GLuint background_texture_id = setup_texture_n_scale_matrix(0, 0, 800, 480, DSANDGRAINS_BACKGROUND_ASSET_PATH, background_scale_matrix);
    GLuint knob1_texture_id = setup_texture_n_scale_matrix(1, 1, 64, 64, DSANDGRAINS_KNOB1_ASSET_PATH, knob1_scale_matrix);
    GLuint knob2_texture_id = setup_texture_n_scale_matrix(1, 1, 48, 48, DSANDGRAINS_KNOB2_ASSET_PATH, knob2_scale_matrix);
    GLuint slider_texture_id = setup_texture_n_scale_matrix(0, 1, 10, 10, DSANDGRAINS_SLIDER1_ASSET_PATH, slider_scale_matrix);
    GLuint system_usage_texture_id = setup_texture_n_scale_matrix(0, 1, 30, 23, DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH, system_usage_scale_matrix);
    GLuint charset_texture_id = setup_texture_n_scale_matrix(0, 1, 30, 23, DSTUDIO_CHAR_TABLE_ASSET_PATH, NULL);
    DSTUDIO_SET_TEXT_SCALE_MATRIX(charset_scale_matrix, 104, 234)
    
    /* Tell to mouse button callback the height of the current active slider */
    slider_texture_scale = 10;
    
    //~ init_system_usage_ui(
        //~ ui_system_usage_p,
        //~ DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH,
        //~ DSTUDIO_CHAR_TABLE_ASSET_PATH,
        //~ 30, 
        //~ 23, 
        //~ 104, 
        //~ 234,
        //~ -0.035,
        //~ 0.889583
    //~ );

    //~ init_instances_ui(
        //~ 7,
        //~ 0.678,
        //~ 0.360416
    //~ );

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
        {-0.222499,  0.254166, 286.0, 335.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION TUNE
        {-0.102500,  0.254166, 334.0, 383.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION PHASE
        {-0.222499,  0.008333, 286.0, 335.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION DEPTH
        {-0.102500,  0.008333, 334.0, 383.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_2}, // SAMPLE: LFO MODULATION SIGNAL
        { 0.042499,  0.254166, 392.0, 441.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD TUNE
        { 0.162499,  0.254166, 440.0, 489.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD PHASE
        { 0.042499,  0.008333, 392.0, 441.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD DEPTH
        { 0.162499,  0.008333, 440.0, 489.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_2}, // SAMPLE: LFO PITCH MOD SIGNAL
        {-0.102500, -0.5375,   334.0, 383.0, 344.0, 393.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: AMOUNT
        { 0.162499, -0.808333, 440.0, 488.0, 409.0, 458.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: PITCH
    };
    
    UIElementSetting voice_knobs_settings_array[DSANDGRAINS_VOICE_KNOBS] = {
        { 0.5475,  0.270833, 586.0, 651.0, 142.0, 207.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : DENSITY
        { 0.3725, -0.129166, 516,   581.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : VOLUME
        { 0.5475, -0.129166, 586.0, 651.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : PAN
    };

    UIElementSetting sliders_dahdsr_settings_array[DSANDGRAINS_DAHDSR_SLIDERS_COUNT] = {
        {-0.14, -0.816666, 338.0, 349.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1,  -0.816666, 354.0, 365.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.06, -0.816666, 370.0, 381.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.02, -0.816666, 386.0, 397.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {0.02, -0.816666,  402.0, 413.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {0.06, -0.816666,  418.0, 429.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    UIElementSetting sliders_dahdsr_pitch_settings_array[DSANDGRAINS_DAHDSR_SLIDERS_COUNT] = {
        {0.0025, -0.558333, 395.0, 406.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0425, -0.558333, 411.0, 422.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0825, -0.558333, 427.0, 438.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1225, -0.558333, 443.0, 454.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1625, -0.558333, 459.0, 470.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.2025, -0.558333, 475.0, 486.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    UIElementSetting sliders_dahdsr_lfo_settings_sarray[DSANDGRAINS_DAHDSR_SLIDERS_COUNT] = {
        {-0.2625, -0.283333, 289.0, 300.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.2225, -0.283333, 305.0, 316.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1825, -0.283333, 321.0, 332.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1425, -0.283333, 337.0, 348.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1025, -0.283333, 353.0, 364.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.0625, -0.283333, 369.0, 380.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1}
    };

    UIElementSetting sliders_dahdsr_lfo_pitch_settings_array[DSANDGRAINS_DAHDSR_SLIDERS_COUNT] = {
        {0.0025, -0.283333, 395.0, 406.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0425, -0.283333, 411.0, 422.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0825, -0.283333, 427.0, 438.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1225, -0.283333, 443.0, 454.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1625, -0.283333, 459.0, 470.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.2025, -0.283333, 475.0, 486.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1}
    };

    UIElementSetting sliders_equalizer_settings_array[DSANDGRAINS_EQUALIZER_SLIDERS_COUNT] = {
        {0.32, -0.475, 522.0, 533.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.36, -0.475, 538.0, 549.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.4,  -0.475, 554.0, 565.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.44, -0.475, 570.0, 581.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.48, -0.475, 586.0, 597.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.52, -0.475, 602.0, 613.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.56, -0.475, 618.0, 629.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.60, -0.475, 634.0, 645.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1}
    };

    /* Inits ui elements */
    UIElementSettingParams params = {0};
    params.callbacks = ui_callbacks;
    params.areas = ui_areas;
    
    /* Background */
    init_ui_elements(&background, background_texture_id, 1, NULL, NULL);

    /* System Usage */
    Vec4 offsets = {-0.035, 0.889583, 0, 0};
    init_ui_elements(&system_usage, system_usage_texture_id, 1, NULL, &offsets);
   
    /* Knobs */
    params.update_callback = update_knob;
    
    params.settings = sample_knobs_settings_array;
    params.array_offset = 0;
    init_ui_elements(&sample_knobs, knob1_texture_id, 8, configure_ui_element, &params);
    
    params.settings = sample_small_knobs_settings_array;
    params.array_offset += 8;
    init_ui_elements(&sample_small_knobs, knob2_texture_id, 10, configure_ui_element, &params);
    
    params.settings = voice_knobs_settings_array;
    params.array_offset += 10;
    init_ui_elements(&voice_knobs, knob1_texture_id, 3, configure_ui_element, &params);
    
    /* Sliders */
    params.update_callback = update_slider;
    
    params.array_offset += 3;
    params.settings = sliders_dahdsr_settings_array;
    init_ui_elements(&sliders_dahdsr, slider_texture_id, DSANDGRAINS_DAHDSR_SLIDERS_COUNT, configure_ui_element, &params);
    
    params.array_offset += DSANDGRAINS_DAHDSR_SLIDERS_COUNT;
    params.settings = sliders_dahdsr_pitch_settings_array;
    init_ui_elements(&sliders_dahdsr_pitch, slider_texture_id, DSANDGRAINS_DAHDSR_SLIDERS_COUNT, configure_ui_element, &params);
    
    params.array_offset += DSANDGRAINS_DAHDSR_SLIDERS_COUNT;
    params.settings = sliders_dahdsr_lfo_settings_sarray;
    init_ui_elements(&sliders_dahdsr_lfo, slider_texture_id, DSANDGRAINS_DAHDSR_SLIDERS_COUNT, configure_ui_element, &params);

    params.array_offset += DSANDGRAINS_DAHDSR_SLIDERS_COUNT;
    params.settings = sliders_dahdsr_lfo_pitch_settings_array;
    init_ui_elements(&sliders_dahdsr_lfo_pitch, slider_texture_id, DSANDGRAINS_DAHDSR_SLIDERS_COUNT, configure_ui_element, &params);

    params.array_offset += DSANDGRAINS_DAHDSR_SLIDERS_COUNT;
    params.settings = sliders_equalizer_settings_array;
    init_ui_elements(&sliders_equalizer, slider_texture_id, DSANDGRAINS_EQUALIZER_SLIDERS_COUNT, configure_ui_element, &params);

    /* Setting shader uniform input ID */
    non_interactive_scale_matrix_id = glGetUniformLocation(non_interactive_program_id, "scale_matrix");
    interactive_scale_matrix_id = glGetUniformLocation(interactive_program_id, "scale_matrix");
    motion_type_id = glGetUniformLocation(interactive_program_id, "motion_type");

    //~ ui_system_usage_scale_matrix_p = &ui_system_usage_p->scale_matrix[0].x;
    //~ ui_text_system_usage_scale_matrix_p = &ui_system_usage_p->ui_text_cpu.scale_matrix[0].x;
}

static void render_background(void * obj, int type) {
    //~ if (type == DSANDGRAINS_BACKGROUND_TYPE_BACKGROUND) {
        //~ render_ui_elements( ((UIElements * ) obj)->texture_id, ((UIElements * ) obj)->vertex_array_object, ((UIElements* ) obj)->index_buffer_object, 1);
    //~ }
    //~ else if (type == DSANDGRAINS_BACKGROUND_TYPE_SYSTEM_USAGE){
        //~ render_ui_elements( ((UISystemUsage * ) obj)->texture_id, ((UISystemUsage * ) obj)->vertex_array_object, ((UISystemUsage * ) obj)->index_buffer_object, 1);
    //~ }
}

static void render_viewport(int mask) {

    glUseProgram(non_interactive_program_id);
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) background_scale_matrix);
        render_ui_elements(&background);
        
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) system_usage_scale_matrix);
        render_ui_elements(&system_usage);
        
        // SYSTEM USAGE
        //~ if (mask & DSTUDIO_RENDER_SYSTEM_USAGE) {
            //~ glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, ui_text_system_usage_scale_matrix_p);
            //~ render_text(&ui_system_usage_p->ui_text_cpu);
            //~ render_text(&ui_system_usage_p->ui_text_mem);
        //~ }
        //~ // INSTANCES
        //~ if (mask & DSTUDIO_RENDER_INSTANCES) {
            //~ glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, &ui_instances_p->lines[0].scale_matrix[0].x);
            //~ for (int i = 0; i < ui_instances_p->lines_number; i++) {
                //~ render_text(&ui_instances_p->lines[i]);
            //~ }
        //~ }

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
        
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) slider_scale_matrix);
            render_ui_elements(&sliders_dahdsr_pitch);
        
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) slider_scale_matrix);
            render_ui_elements(&sliders_dahdsr_lfo);
        
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) slider_scale_matrix);
            render_ui_elements(&sliders_dahdsr_lfo_pitch);
        
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) slider_scale_matrix);
            render_ui_elements(&sliders_equalizer);
        }
}

// Should be splitted
void * ui_thread(void * arg) {
    int redraw_all = 0;
    unsigned int instances_count;
    unsigned int instances_last_id;
    unsigned long int previous_timestamp = 0;

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
            if (areas_index >= 0) {
                glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
                render_viewport(render_mask);
            }
            
            //~ // UPDATE AND RENDER TEXT
            //~ sem_wait(&ui_system_usage_p->mutex);
            //~ if (ui_system_usage_p->update && !redraw_all) {
                //~ update_text(&ui_system_usage_p->ui_text_cpu);
                //~ update_text(&ui_system_usage_p->ui_text_mem);
                //~ glScissor(402, 438, 48, 31);
                //~ render_viewport(DSTUDIO_RENDER_SYSTEM_USAGE);
                //~ ui_system_usage_p->update = 0;
            //~ }
            //~ sem_post(&ui_system_usage_p->mutex);

            //~ sem_wait(&ui_instances_p->mutex);
            //~ if (ui_instances_p->update && !redraw_all) {
                //~ update_instances_text();
                //~ glScissor(669, 254, 117, 79);
                //~ render_viewport(DSTUDIO_RENDER_INSTANCES);
                //~ ui_instances_p->update = 0;
            //~ }
            //~ sem_post(&ui_instances_p->mutex);
        }
        swap_window_buffer();
        listen_events();
    }
    
    //~ sem_wait(&ui_system_usage_p->mutex);
    //~ ui_system_usage_p->cut_thread = 1;
    //~ sem_post(&ui_system_usage_p->mutex);
    
    //exit_instances_thread();
    destroy_context();
    return NULL;
}
