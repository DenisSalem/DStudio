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
static GLuint interactive_scale_matrix_id, non_interactive_scale_matrix_id;
static GLuint motion_type_id;

static const GLfloat *background_scale_matrix_p;
static const GLfloat *ui_system_usage_scale_matrix_p;

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
        background->scale_matrix
    );
}

static void render_background(void * obj, int type) {
    if (type == DSANDGRAINS_BACKGROUND_TYPE_BACKGROUND) {
        render_background_element( ((UIBackground * ) obj)->texture_id, ((UIBackground * ) obj)->vertex_array_object, ((UIBackground * ) obj)->index_buffer_object);
    }
    else if (type == DSANDGRAINS_BACKGROUND_TYPE_SYSTEM_USAGE){
        render_background_element( ((UISystemUsage * ) obj)->texture_id, ((UISystemUsage * ) obj)->vertex_array_object, ((UISystemUsage * ) obj)->index_buffer_object);
    }
}

static void render_viewport() {
    glUseProgram(non_interactive_program_id);
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, background_scale_matrix_p);
        render_background(background_p, DSANDGRAINS_BACKGROUND_TYPE_BACKGROUND);

        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, ui_system_usage_scale_matrix_p);
        render_background(ui_system_usage_p, DSANDGRAINS_BACKGROUND_TYPE_SYSTEM_USAGE);
        
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

    UI * ui = arg;
    background_p = &ui->background;
    sample_knobs_p = &ui->sample_knobs;
    sample_small_knobs_p = &ui->sample_small_knobs;
    sliders_dahdsr_p = &ui->sliders_dahdsr;
    sliders_dahdsr_pitch_p = &ui->sliders_dahdsr_pitch;
    sliders_dahdsr_lfo_p = &ui->sliders_dahdsr_lfo;
    sliders_dahdsr_lfo_pitch_p = &ui->sliders_dahdsr_lfo_pitch;
    sliders_equalizer_p = &ui->sliders_equalizer;
    ui_system_usage_p = &ui->system_usage;
    
    voice_knobs_p = &ui->voice_knobs;
    ui_areas = &ui->areas[0];
    ui_callbacks = &ui->callbacks[0];

    init_context("DSANDGRAINS",DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
        
    set_mouse_button_callback(mouse_button_callback);
    set_cursor_position_callback(cursor_position_callback);
    	
    DSTUDIO_EXIT_IF_FAILURE(load_extensions())
    
    create_shader_program(&interactive_program_id, &non_interactive_program_id);
    init_background(background_p);
    init_system_usage_ui(ui_system_usage_p, DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH, 78, 23, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    
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

    interactive_scale_matrix_id = glGetUniformLocation(interactive_program_id, "scale_matrix");
    non_interactive_scale_matrix_id = glGetUniformLocation(non_interactive_program_id, "scale_matrix");
    motion_type_id = glGetUniformLocation(interactive_program_id, "motion_type");

    background_scale_matrix_p = &background_p->scale_matrix[0].x;
    ui_system_usage_scale_matrix_p = &ui_system_usage_p->scale_matrix[0].x;

    sample_knobs_scale_matrix_p = &sample_knobs_p->scale_matrix[0].x;
    sample_small_knobs_scale_matrix_p = &sample_small_knobs_p->scale_matrix[0].x;
    voice_knobs_scale_matrix_p = &voice_knobs_p->scale_matrix[0].x;

    sliders_dahdsr_scale_matrix_p = &sliders_dahdsr_p->scale_matrix[0].x;
    sliders_dahdsr_pitch_scale_matrix_p = &sliders_dahdsr_pitch_p->scale_matrix[0].x;
    sliders_dahdsr_lfo_scale_matrix_p = &sliders_dahdsr_lfo_p->scale_matrix[0].x;
    sliders_dahdsr_lfo_pitch_scale_matrix_p = &sliders_dahdsr_lfo_pitch_p->scale_matrix[0].x;
    sliders_equalizer_scale_matrix_p = &sliders_equalizer_p->scale_matrix[0].x;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    
    while (do_no_exit_loop()) {
        usleep(framerate);

        /* MANAGE INSTANCES */
        //count_instances(INSTANCES_DIRECTORY, &instances_count, &instances_last_id);
        //clock_gettime(CLOCK_REALTIME, &tStruct);

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
    destroy_context();
    return NULL;
}
