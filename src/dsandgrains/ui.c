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

#include "../common.h"
#include "../extensions.h"
#include "../window_management.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ui.h"

unsigned int framerate = DSTUDIO_FRAMERATE;

UIElementsStruct g_ui_elements_struct = {0};
UIElements * g_ui_elements_array = (UIElements *) &g_ui_elements_struct;

Vec2 background_scale_matrix[2] = {0};
Vec2 knob1_64_scale_matrix[2] = {0};
Vec2 knob1_48_scale_matrix[2] = {0};

inline static void init_background() {
    g_ui_elements_struct.background.type = DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND;
    g_ui_elements_struct.background.render = 1;
    g_ui_elements_struct.background.instance_alphas_buffer = dstudio_alloc(sizeof(GLfloat));
    g_ui_elements_struct.background.instance_alphas_buffer[0] = 1.0;
    g_ui_elements_struct.background.instance_motions_buffer = dstudio_alloc(sizeof(GLfloat));
    g_ui_elements_struct.background.instance_offsets_buffer = dstudio_alloc(sizeof(Vec4));
    g_ui_elements_struct.background.scissor.x = 0;
    g_ui_elements_struct.background.scissor.y = 0;
    g_ui_elements_struct.background.scissor.width = g_dstudio_viewport_width;
    g_ui_elements_struct.background.scissor.height = g_dstudio_viewport_height;
    g_ui_elements_struct.background.texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_NONE,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height, 
        DSANDGRAINS_BACKGROUND_ASSET_PATH,
        background_scale_matrix
    );
    g_ui_elements_struct.background.scale_matrix = &background_scale_matrix[0];
    g_ui_elements_struct.background.count = 1;
    init_ui_elements(
        DSTUDIO_FLAG_NONE,
        &g_ui_elements_struct.background
    );
}

inline static void init_knobs() {
    GLuint knob_textures_ids[2] = {0};
        
    knob_textures_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING,
        DSTUDIO_KNOB_1_64_WIDTH,
        DSTUDIO_KNOB_1_64_HEIGHT, 
        DSTUDIO_KNOB_1_64x64_TEXTURE_PATH,
        knob1_64_scale_matrix
    );

    init_ui_elements_array(
        &g_ui_elements_struct.knob_sample_start,
        &knob_textures_ids[0],
        &knob1_64_scale_matrix[0],
        DSANDGRAINS_SAMPLE_KNOBS_POS_X,
        DSANDGRAINS_SAMPLE_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_64_AREA_WIDTH,
        DSTUDIO_KNOB_1_64_AREA_HEIGHT,
        DSANDGRAINS_SAMPLE_KNOBS_OFFSET_X,
        DSANDGRAINS_SAMPLE_KNOBS_OFFSET_Y,
        DSANDGRAINS_SAMPLE_KNOBS_COLUMNS,
        8,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB
    );
    
    init_ui_elements_array(
        &g_ui_elements_struct.knob_voice_volume,
        &knob_textures_ids[0],
        &knob1_64_scale_matrix[0],
        DSANDGRAINS_VOICE_KNOBS_POS_X,
        DSANDGRAINS_VOICE_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_64_AREA_WIDTH,
        DSTUDIO_KNOB_1_64_AREA_HEIGHT,
        DSANDGRAINS_VOICE_KNOBS_OFFSET_X,
        DSANDGRAINS_VOICE_KNOBS_OFFSET_Y,
        DSANDGRAINS_VOICE_KNOBS_COLUMNS,
        4,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB
    );
    
    knob_textures_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING,
        DSTUDIO_KNOB_1_48_WIDTH,
        DSTUDIO_KNOB_1_48_HEIGHT, 
        DSTUDIO_KNOB_1_48x48_TEXTURE_PATH,
        knob1_48_scale_matrix
    );
    
    init_ui_elements_array(
        &g_ui_elements_struct.knob_sample_amount,
        &knob_textures_ids[0],
        &knob1_48_scale_matrix[0],
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_X,
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_48_AREA_WIDTH,
        DSTUDIO_KNOB_1_48_AREA_HEIGHT,
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_X,
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_OFFSET_Y,
        DSANDGRAINS_AMOUNT_PITCH_KNOBS_COLUMNS,
        2,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB
    );
    
    init_ui_elements_array(
        &g_ui_elements_struct.knob_sample_lfo_tune,
        &knob_textures_ids[0],
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
        DSTUDIO_UI_ELEMENT_TYPE_KNOB
    );
    
    init_ui_elements_array(
        &g_ui_elements_struct.knob_sample_lfo_pitch_tune,
        &knob_textures_ids[0],
        &knob1_48_scale_matrix[0],
        DSANDGRAINS_LFO_PITCH_KNOBS_POS_X,
        DSANDGRAINS_LFO_PITCH_KNOBS_POS_Y,
        DSTUDIO_KNOB_1_48_AREA_WIDTH,
        DSTUDIO_KNOB_1_48_AREA_HEIGHT,
        DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_X,
        DSANDGRAINS_LFO_PITCH_KNOBS_OFFSET_Y,
        DSANDGRAINS_LFO_PITCH_KNOBS_COLUMNS,
        4,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_KNOB
    );
}

static void init_ui() {
    g_scale_matrix_id = glGetUniformLocation(g_shader_program_id, "scale_matrix");
    init_background();
    init_knobs();
    //sem_init(&g_text_pointer_context.mutex, 0, 1);
}

void * ui_thread(void * arg) {
    (void) arg;
    init_context(
        g_application_name,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height
    );
    set_mouse_button_callback(manage_mouse_button);
    set_cursor_position_callback(manage_cursor_position);
    	
    DSTUDIO_EXIT_IF_FAILURE(load_extensions())
    
    create_shader_program(&g_shader_program_id);
    
    g_motion_type_location = glGetUniformLocation(g_shader_program_id, "motion_type");
    
    init_ui();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glUseProgram(g_shader_program_id);
    while (do_no_exit_loop()) {
        usleep(framerate);
        render_viewport(need_to_redraw_all());
        swap_window_buffer();
        listen_events();
    }
    
    destroy_context();
    return NULL;
}
