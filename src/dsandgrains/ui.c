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
#include "../interactive_list.h"
#include "../knobs.h"
#include "../text.h"
#include "../voices.h"
#include "instances.h"
#include "ui.h"
#include "initialization_macros.h"

/* Background */
static UIElements background = {0};
static Vec2 background_scale_matrix[2] = {0};

/* Charsets */
static Vec2 charset_scale_matrix[2] = {0};
static Vec2 charset_small_scale_matrix[2] = {0};

/* Buttons management */
ButtonsManagement g_buttons_management = {0};

/* Arrows */
static UIElements arrow_instances_top = {0};
static UIElements arrow_instances_bottom = {0};
static UIElements arrow_voices_top = {0};
static UIElements arrow_voices_bottom = {0};
static UIElements arrow_samples_top = {0};
static UIElements arrow_samples_bottom = {0};
static Vec2 arrow_instances_scale_matrix[2] = {0};

static Vec2 scrollable_list_shadow_scale_matrix[2] = {0};

/* Instances */
static UIElements instances[7] = {0};
static UIElements instances_shadow = {0};

/* Voices */
static UIElements voices[7] = {0};

/* System Usage */
static UIElements system_usage = {0};
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
static UIElements sliders_equalizer = {0};
static Vec2 slider_scale_matrix[2] = {0};

UIArea ui_areas[DSANDGRAINS_UI_ELEMENTS_COUNT] = {0};
UICallback ui_callbacks[DSANDGRAINS_UI_ELEMENTS_COUNT] = {0};

static useconds_t framerate = 20000;
static char areas_index = -1;

static GLuint interactive_program_id, non_interactive_program_id;
static GLuint interactive_scale_matrix_id, non_interactive_scale_matrix_id;
static GLuint motion_type_id;
static GLuint no_texture_id;

static GLfloat motion_type;

static ButtonStates button_settings_array[DSANDGRAINS_BUTTONS_COUNT] = {0};

#include "../ui_statics.h"

static void init_ui() {
    //--- Local variables ---------------------------------------------/
    
    // Setting arrays and configuration parameters
    UIElementSetting * sliders_settings_array = 0;
    UIElementSetting instance_shadows_settings_array[DSANDGRAINS_SCROLLABLE_LIST_SIZE] = {0};
 ///// Declared and initialized in SETUP_KNOBS_SETTING_ARRAYS macro ////
 // UIElementSetting sample_knobs_settings_array[DSANDGRAINS_SAMPLE_KNOBS];
 // UIElementSetting sample_small_knobs_settings_array[DSANDGRAINS_SAMPLE_SMALL_KNOBS];
 // UIElementSetting voice_knobs_settings_array[DSANDGRAINS_VOICE_KNOBS];
 ///////////////////////////////////////////////////////////////////////
    UIElementSetting buttons_settings_array = {0};

    UITextSettingParams text_params;
    UIElementSettingParams params = {0};
    params.callbacks = ui_callbacks;
    params.areas = ui_areas;
    
    // Shared texture ids
    GLuint background_texture_id;
    GLuint knob1_texture_id;
    GLuint knob2_texture_id;
    GLuint slider_texture_id;
    GLuint system_usage_texture_id;
    GLuint charset_texture_id;
    GLuint charset_small_texture_id;
    
    Vec4 offsets = {0};
    //--- End local variables -----------------------------------------/
    
    // Setting shader uniform input ID 
    non_interactive_scale_matrix_id = glGetUniformLocation(non_interactive_program_id, "scale_matrix");
    no_texture_id = glGetUniformLocation(non_interactive_program_id, "no_id");
    interactive_scale_matrix_id = glGetUniformLocation(interactive_program_id, "scale_matrix");
    motion_type_id = glGetUniformLocation(interactive_program_id, "motion_type");
    
    /*
     * There is way too much initialization calls, for readability
     * most of them has been put in initialization_macros.h
     */
     
    LOAD_SHARED_TEXTURE_AND_PREPARE_SHARED_SCALE_MATRICES
    SETUP_BUTTONS_SETTING_ARRAYS
    SETUP_KNOBS_SETTING_ARRAYS
    INIT_BACKGROUND
    INIT_SYSTEM_USAGE
    INIT_INSTANCE_SCROLLABLE_LIST
    INIT_VOICE_SCROLLABLE_LIST
    INIT_SCROLLABLE_LIST_ARROWS

    /* Instances Shadow */
    instance_shadows_settings_array[0].gl_x = DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SHADOW_X_POS;
    instance_shadows_settings_array[0].gl_y = DSANDGRAINS_INSTANCE_SCROLLABLE_LIST_SHADOW_Y_POS;
    params.update_callback = 0;
    params.settings = &instance_shadows_settings_array[0];
    init_ui_elements(
        DSTUDIO_FLAG_NONE,
        &instances_shadow,
        0, // There is no texture id: we're rendering solid quad
        DSANDGRAINS_SCROLLABLE_LIST_SIZE,
        configure_ui_interactive_list,
        &params
    );

    params.array_offset += DSANDGRAINS_SCROLLABLE_LIST_SIZE;
    
    INIT_KNOBS
    INIT_SLIDERS

    init_buttons_management(
        &g_buttons_management,
        &button_settings_array[0],
        DSANDGRAINS_BUTTONS_COUNT
    );
    
    DSTUDIO_EXIT_IF_FAILURE(
        pthread_create(
            &g_buttons_management.thread_id,
            NULL,
            buttons_management_thread,
            &g_buttons_management
        )
    )
}

void render_viewport(int mask) {
    GLfloat no_texture = 0.0;
    glUseProgram(non_interactive_program_id);
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) background_scale_matrix);
        render_ui_elements(&background);
        
        // SYSTEM USAGE
        glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) system_usage_scale_matrix);
        render_ui_elements(&system_usage);
        
        // CPU & MEM USAGE
        if (mask & DSTUDIO_RENDER_SYSTEM_USAGE) {
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) charset_scale_matrix);
            render_ui_elements(&g_cpu_usage);
            render_ui_elements(&g_mem_usage);
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
            no_texture = 1.0;
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) scrollable_list_shadow_scale_matrix);
            glUniformMatrix2fv(no_texture_id, 1, GL_FALSE, (float *) &no_texture);
            render_ui_elements(&instances_shadow);
            
            no_texture = 0.0;
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) charset_small_scale_matrix);
            glUniformMatrix2fv(no_texture_id, 1, GL_FALSE, (float *) &no_texture);

            for (unsigned int i = 0; i < g_ui_instances.lines_number; i++) {
                render_ui_elements(&g_ui_instances.lines[i]);
            }
        }
        
        // VOICES
        if (mask & DSTUDIO_RENDER_VOICES) {
            glUniformMatrix2fv(non_interactive_scale_matrix_id, 1, GL_FALSE, (float *) charset_small_scale_matrix);
            for (unsigned int i = 0; i < g_ui_voices.lines_number; i++) {
                render_ui_elements(&g_ui_voices.lines[i]);
            }
        }


    glUseProgram(interactive_program_id);
        // KNOBS
        if (mask & DSTUDIO_RENDER_KNOBS) {
            motion_type = 0.0;
            glUniform1f(motion_type_id, motion_type);
            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) knob1_scale_matrix);
            render_ui_elements(&sample_knobs);
            render_ui_elements(&voice_knobs);

            glUniformMatrix2fv(interactive_scale_matrix_id, 1, GL_FALSE, (float *) knob2_scale_matrix);
            render_ui_elements(&sample_small_knobs);
        
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

void * ui_thread(void * arg) {
    (void) arg;
    init_context("DSANDGRAINS", DSTUDIO_VIEWPORT_WIDTH, DSTUDIO_VIEWPORT_HEIGHT);
    set_mouse_button_callback(mouse_button_callback);
    set_cursor_position_callback(cursor_position_callback);
    	
    DSTUDIO_EXIT_IF_FAILURE(load_extensions())
    
    create_shader_program(&interactive_program_id, &non_interactive_program_id);
    
    init_ui();

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
                &g_buttons_management,
                ui_callbacks,
                mouse_state,
                ui_areas
            );

            // Check for knob or slider to render
            if (areas_index >= 0) {
                glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
                render_viewport(render_mask);
            }
            
            // UPDATE AND RENDER TEXT            
            update_and_render(
                &g_ui_system_usage.mutex,
                &g_ui_system_usage.update,
                update_ui_system_usage,
                402, 438, 48, 31,
                DSTUDIO_RENDER_SYSTEM_USAGE
            );
            
            update_and_render(
                &g_ui_instances.mutex,
                &g_ui_instances.update,
                update_instances_text,
                669, 254, 117, 79,
                DSTUDIO_RENDER_INSTANCES
            );
            
            update_and_render(
                &g_ui_voices.mutex,
                &g_ui_voices.update,
                update_voices_text,
                669, 143, 117, 79,
                DSTUDIO_RENDER_VOICES
            );
        }
                
        swap_window_buffer();
        listen_events();
    }
    
    sem_wait(&g_ui_system_usage.mutex);
    g_ui_system_usage.cut_thread = 1;
    sem_post(&g_ui_system_usage.mutex);

    sem_wait(&g_buttons_management.mutex);
    g_buttons_management.cut_thread = 1;
    sem_post(&g_buttons_management.mutex);
    DSTUDIO_EXIT_IF_FAILURE(pthread_join(g_buttons_management.thread_id, NULL))

    exit_instances_thread();
    destroy_context();
    return NULL;
}
