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

GLuint g_shader_program_id = 0;
GLuint g_scale_matrix_id = 0;
unsigned int framerate = DSTUDIO_FRAMERATE;

UIElementsArray g_ui_elements_array = {0};

Vec2 background_scale_matrix[2] = {0};

inline static void init_background() {
    g_ui_elements_array.background.type = DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND;
    g_ui_elements_array.background.render = 1;
    g_ui_elements_array.background.instance_alphas_buffer = dstudio_alloc(sizeof(GLfloat));
    g_ui_elements_array.background.instance_alphas_buffer[0] = 1.0;
    g_ui_elements_array.background.instance_motions_buffer = dstudio_alloc(sizeof(GLfloat));
    g_ui_elements_array.background.instance_offsets_buffer = dstudio_alloc(sizeof(Vec4));
    g_ui_elements_array.background.scissor.x = 0;
    g_ui_elements_array.background.scissor.y = 0;
    g_ui_elements_array.background.scissor.width = g_dstudio_viewport_width;
    g_ui_elements_array.background.scissor.height = g_dstudio_viewport_height;
    g_ui_elements_array.background.texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_NONE,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height, 
        DSANDGRAINS_BACKGROUND_ASSET_PATH,
        background_scale_matrix
    );
    g_ui_elements_array.background.scale_matrix = &background_scale_matrix[0];
    g_ui_elements_array.background.count = 1;
    init_ui_elements(
        DSTUDIO_FLAG_NONE,
        &g_ui_elements_array.background
    );
}

static void init_ui() {
    printf("sizeof(UIElements) == %lu\n", sizeof(UIElements));
    g_scale_matrix_id = glGetUniformLocation(g_shader_program_id, "scale_matrix");
    init_background();
    //sem_init(&g_text_pointer_context.mutex, 0, 1);
}

DEFINE_RENDER_VIEWPORT

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
    
    init_ui();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glUseProgram(g_shader_program_id);
    while (do_no_exit_loop()) {
        usleep(framerate);
        render_viewport();
        swap_window_buffer();
        listen_events();
    }
    
    destroy_context();
    return NULL;
}
