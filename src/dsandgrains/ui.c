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
#include "../knobs.h"
#include "instances.h"
#include "ui.h"

static UIKnobs * sample_knobs_p;
static UIKnobs * sample_small_knobs_p;
static UISliders * sliders_dahdsr_p;
static UISliders * sliders_dahdsr_pitch_p;
static UISliders * sliders_dahdsr_lfo_p;
static UISliders * sliders_dahdsr_lfo_pitch_p;
static UISliders * sliders_equalizer_p;
static UIKnobs * voice_knobs_p;
static UISystemUsage * ui_system_usage_p;

static UIBackground * background_p;
static UIArea * ui_areas;
static UICallback * ui_callbacks;
static UICallback active_ui_element = {0};
static Vec2 active_ui_element_center;
static Vec2 active_slider_range;
static useconds_t framerate = 20000;
static char first_render = 1;
static char areas_index = -1;

static GLint scissor_x, scissor_y;
static GLsizei scissor_width, scissor_height;

static GLuint interactive_program_id, non_interactive_program_id;
static GLuint interactive_scale_matrix_id, non_interactive_scale_matrix_id, background_element_offset_id;
static GLuint motion_type_id;

static const GLfloat * background_scale_matrix_p;
static const GLfloat * ui_system_usage_scale_matrix_p;
static const GLfloat * ui_text_cpu_usage_scale_matrix_p;
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

static void init_background(UIBackground * background) {
    background_p->instance_offsets_buffer.x = 0;
    background_p->instance_offsets_buffer.y = 0;
    background_p->instance_offsets_buffer.z = 0;
    background_p->instance_offsets_buffer.w = 0;
    init_background_element(
        background->vertex_indexes,
        background->vertexes_attributes,
        &background->index_buffer_object,
        &background->vertex_buffer_object,
        DSANDGRAINS_BACKGROUND_ASSET_PATH,
        &background->texture,
        0,
        &background->texture_id,
        &background->vertex_array_object,
        800,
        480,
        DSANDGRAINS_VIEWPORT_WIDTH,
        DSANDGRAINS_VIEWPORT_HEIGHT,
        background->scale_matrix,
        &background->instance_offsets,
        &background->instance_offsets_buffer,
        1
    );
}

static void init_ui(UI * ui) {
    background_p = &ui->background;
    sample_knobs_p = &ui->sample_knobs;
    sample_small_knobs_p = &ui->sample_small_knobs;
    voice_knobs_p = &ui->voice_knobs;
    sliders_dahdsr_p = &ui->sliders_dahdsr;
    sliders_dahdsr_pitch_p = &ui->sliders_dahdsr_pitch;
    sliders_dahdsr_lfo_p = &ui->sliders_dahdsr_lfo;
    sliders_dahdsr_lfo_pitch_p = &ui->sliders_dahdsr_lfo_pitch;
    sliders_equalizer_p = &ui->sliders_equalizer;
    ui_areas = &ui->areas[0];
    ui_callbacks = &ui->callbacks[0];
    ui_system_usage_p = &ui->system_usage;
    
    init_background(background_p);
    init_system_usage_ui(ui_system_usage_p, DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH, DSANDGRAINS_CHAR_TABLE_ASSET_PATH, 78, 23, 104, 234, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);

    init_knobs_cpu_side(sample_knobs_p, 8, 64, DSANDGRAINS_KNOB1_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_knobs_cpu_side(sample_small_knobs_p, 10, 48, DSANDGRAINS_KNOB2_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_knobs_cpu_side(voice_knobs_p, 3, 64, DSANDGRAINS_KNOB1_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_pitch_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_lfo_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_lfo_pitch_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_equalizer_p, 8, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);

    InitUIElementArray init_knobs_array[DSANDGRAINS_KNOBS_COUNT] = {
        {-0.8675, 0.25, 20.0,  85.0,  147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: START
        {-0.7075, 0.25, 84.0,  149.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: END
        {-0.5475, 0.25, 148.0, 213.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: GRAIN SIZE
        {-0.3875, 0.25, 212.0, 277.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: TRANSIENT THRESOLD
        {-0.8675, -.15, 20.0,  85.0,  243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LOOP DURATION
        {-0.7075, -.15, 84.0,  149.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: CLOUD SIZE
        {-0.5475, -.15, 148.0, 213.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: DISTRIBUTION BALANCE
        {-0.3875, -.15, 212.0, 277.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: SMOOTH TRANSITION
        
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
        { 0.5475,  0.270833, 586.0, 651.0, 142.0, 207.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : DENSITY
        { 0.3725, -0.129166, 516,   581.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : VOLUME
        { 0.5475, -0.129166, 586.0, 651.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : PAN
    };

    InitUIElementArray * init_knob_array_p;
    
    for (int i = 0; i < DSANDGRAINS_SAMPLE_KNOBS; i++) {
        init_knob_array_p = &init_knobs_array[i];
        DSTUDIO_INIT_KNOB(sample_knobs_p, i, gl_x, gl_y, i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    for (int i = 0; i < DSANDGRAINS_SAMPLE_SMALL_KNOBS; i++) {
        init_knob_array_p = &init_knobs_array[8+i];
        DSTUDIO_INIT_KNOB(sample_small_knobs_p, i, gl_x, gl_y, 8+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    for (int i = 0; i < DSANDGRAINS_VOICE_KNOBS; i++) {
        init_knob_array_p = &init_knobs_array[18+i];
        DSTUDIO_INIT_KNOB(voice_knobs_p, i, gl_x, gl_y, 18+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray * init_slider_array_p;

    InitUIElementArray init_sliders_dahdsr_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {-0.14, -0.816666, 338.0, 349.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1,  -0.816666, 354.0, 365.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.06, -0.816666, 370.0, 381.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.02, -0.816666, 386.0, 397.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {0.02, -0.816666,  402.0, 413.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {0.06, -0.816666,  418.0, 429.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_p, i, gl_x, gl_y, 21+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    InitUIElementArray init_sliders_dahdsr_pitch_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {0.0025, -0.558333, 395.0, 406.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0425, -0.558333, 411.0, 422.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0825, -0.558333, 427.0, 438.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1225, -0.558333, 443.0, 454.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1625, -0.558333, 459.0, 470.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.2025, -0.558333, 475.0, 486.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_pitch_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_pitch_p, i, gl_x, gl_y, 27+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray init_sliders_dahdsr_lfo_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {-0.2625, -0.283333, 289.0, 300.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.2225, -0.283333, 305.0, 316.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1825, -0.283333, 321.0, 332.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1425, -0.283333, 337.0, 348.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1025, -0.283333, 353.0, 364.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.0625, -0.283333, 369.0, 380.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_lfo_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_lfo_p, i, gl_x, gl_y, 33+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray init_sliders_dahdsr_lfo_pitch_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {0.0025, -0.283333, 395.0, 406.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0425, -0.283333, 411.0, 422.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0825, -0.283333, 427.0, 438.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1225, -0.283333, 443.0, 454.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1625, -0.283333, 459.0, 470.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.2025, -0.283333, 475.0, 486.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_lfo_pitch_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_lfo_pitch_p, i, gl_x, gl_y, 39+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray init_sliders_equalizer_array[DSANDGRAINS_EQUALIZER_SLIDERS_COUNT] = {
        {0.32, -0.475, 522.0, 533.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.36, -0.475, 538.0, 549.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.4,  -0.475, 554.0, 565.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.44, -0.475, 570.0, 581.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.48, -0.475, 586.0, 597.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.52, -0.475, 602.0, 613.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.56, -0.475, 618.0, 629.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.60, -0.475, 634.0, 645.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_EQUALIZER_SLIDERS_COUNT; i++) {
        init_slider_array_p = &init_sliders_equalizer_array[i];
        DSTUDIO_INIT_SLIDER(sliders_equalizer_p, i, gl_x, gl_y, 45+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    init_knobs_gpu_side(sample_knobs_p);
    init_knobs_gpu_side(sample_small_knobs_p);
    init_knobs_gpu_side(voice_knobs_p);
    init_sliders_gpu_side(sliders_dahdsr_p);
    init_sliders_gpu_side(sliders_dahdsr_pitch_p);
    init_sliders_gpu_side(sliders_dahdsr_lfo_p);
    init_sliders_gpu_side(sliders_dahdsr_lfo_pitch_p);
    init_sliders_gpu_side(sliders_equalizer_p);
    
    finalize_knobs(sample_knobs_p);
    finalize_knobs(sample_small_knobs_p);
    finalize_knobs(voice_knobs_p);
    finalize_sliders(sliders_dahdsr_p);
    finalize_sliders(sliders_dahdsr_pitch_p);
    finalize_sliders(sliders_dahdsr_lfo_p);
    finalize_sliders(sliders_dahdsr_lfo_pitch_p);
    finalize_sliders(sliders_equalizer_p);

    non_interactive_scale_matrix_id = glGetUniformLocation(non_interactive_program_id, "scale_matrix");
    interactive_scale_matrix_id = glGetUniformLocation(interactive_program_id, "scale_matrix");
    motion_type_id = glGetUniformLocation(interactive_program_id, "motion_type");

    background_scale_matrix_p = &background_p->scale_matrix[0].x;
    ui_system_usage_scale_matrix_p = &ui_system_usage_p->scale_matrix[0].x;
    ui_text_cpu_usage_scale_matrix_p = &ui_system_usage_p->ui_text_cpu.scale_matrix[0].x;

    sample_knobs_scale_matrix_p = &sample_knobs_p->scale_matrix[0].x;
    sample_small_knobs_scale_matrix_p = &sample_small_knobs_p->scale_matrix[0].x;
    voice_knobs_scale_matrix_p = &voice_knobs_p->scale_matrix[0].x;

    sliders_dahdsr_scale_matrix_p = &sliders_dahdsr_p->scale_matrix[0].x;
    sliders_dahdsr_pitch_scale_matrix_p = &sliders_dahdsr_pitch_p->scale_matrix[0].x;
    sliders_dahdsr_lfo_scale_matrix_p = &sliders_dahdsr_lfo_p->scale_matrix[0].x;
    sliders_dahdsr_lfo_pitch_scale_matrix_p = &sliders_dahdsr_lfo_pitch_p->scale_matrix[0].x;
    sliders_equalizer_scale_matrix_p = &sliders_equalizer_p->scale_matrix[0].x;
}

static void render_background(void * obj, int type) {
    if (type == DSANDGRAINS_BACKGROUND_TYPE_BACKGROUND) {
        render_ui_elements( ((UIBackground * ) obj)->texture_id, ((UIBackground * ) obj)->vertex_array_object, ((UIBackground * ) obj)->index_buffer_object, 1);
    }
    else if (type == DSANDGRAINS_BACKGROUND_TYPE_SYSTEM_USAGE){
        render_ui_elements( ((UISystemUsage * ) obj)->texture_id, ((UISystemUsage * ) obj)->vertex_array_object, ((UISystemUsage * ) obj)->index_buffer_object, 1);
    }
}

static void render_viewport() {
    glUseProgram(non_interactive_program_id);
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, background_scale_matrix_p);
        render_background(background_p, DSANDGRAINS_BACKGROUND_TYPE_BACKGROUND);

        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, ui_system_usage_scale_matrix_p);
        render_background(ui_system_usage_p, DSANDGRAINS_BACKGROUND_TYPE_SYSTEM_USAGE);

        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, ui_text_cpu_usage_scale_matrix_p);
        render_text(&ui_system_usage_p->ui_text_cpu);
        
    glUseProgram(interactive_program_id);
        // KNOBS
        motion_type = 0.0;
        glUniform1f(motion_type_id, motion_type);

        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sample_knobs_scale_matrix_p);
        render_knobs(sample_knobs_p);
        
        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sample_small_knobs_scale_matrix_p);
        render_knobs(sample_small_knobs_p);
        
        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, voice_knobs_scale_matrix_p);
        render_knobs(voice_knobs_p);

        // SLIDERS
        motion_type = 1.0;
        glUniform1f(motion_type_id, motion_type);

        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sliders_dahdsr_scale_matrix_p);
        render_sliders(sliders_dahdsr_p);
        
        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sliders_dahdsr_pitch_scale_matrix_p);
        render_sliders(sliders_dahdsr_pitch_p);
        
        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sliders_dahdsr_lfo_scale_matrix_p);
        render_sliders(sliders_dahdsr_lfo_p);
        
        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sliders_dahdsr_lfo_pitch_scale_matrix_p);
        render_sliders(sliders_dahdsr_lfo_pitch_p);

        glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, sliders_equalizer_scale_matrix_p);
        render_sliders(sliders_equalizer_p);
}

// Should be splitted
void * ui_thread(void * arg) {
    unsigned int instances_count;
    unsigned int instances_last_id;
    unsigned long int previous_timestamp = 0;
    //timespec tStruct = {0};

    init_context("DSANDGRAINS",DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
        
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
        
        /* MANAGE INSTANCES */
        //count_instances(INSTANCES_DIRECTORY, &instances_count, &instances_last_id);
        //clock_gettime(CLOCK_REALTIME, &tStruct);

        // UPDATE AND RENDER TEXT
        if (ui_system_usage_p->update) {
            update_text(&ui_system_usage_p->ui_text_cpu);
            glScissor(0, 0, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
            render_viewport();
        }
        
        /* RENDER */
        
        if (need_to_redraw_all()) {
            glScissor(0, 0, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
            render_viewport();
        }
        else if (areas_index >= 0) {
            glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
            render_viewport();
        }
        swap_window_buffer();
        listen_events();
    }
    
    sem_wait(&ui_system_usage_p->mutex);
    ui_system_usage_p->cut_thread = 1;
    destroy_context();
    sem_post(&ui_system_usage_p->mutex);

    return NULL;
}
