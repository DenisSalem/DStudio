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

#ifdef DSTUDIO_USE_GLFW3
    #include <GLFW/glfw3.h>
#endif

#include <stdlib.h>
#include <unistd.h>

#include "../common.h"
#include "../knobs.h"
#include "ui.h"

static UIKnobs * sample_knobs_p;
static UIKnobs * sample_small_knobs_p;
static UISliders * sliders_dahdsr_p;
static UIKnobs * voice_knobs_p;

static UIBackground * background_p;
static UIArea * ui_areas;
static UICallback * ui_callbacks;
static UICallback active_ui_element = {0};
static Vec2 active_ui_element_center;
static Vec2 active_slider_range;
static useconds_t framerate = 20000;
static char first_render = 1;
static char window_visible = 0;
static char areas_index = -1;

static GLint scissor_x, scissor_y;
static GLsizei scissor_width, scissor_height;

static GLuint interactive_program_id, non_interactive_program_id;
static GLuint scale_matrix_id;
static GLuint motion_type_id;

static const GLfloat * sample_knobs_scale_matrix_p;
static const GLfloat * sample_small_knobs_scale_matrix_p;
static const GLfloat * sliders_dahdsr_scale_matrix_p;
static const GLfloat * voice_knobs_scale_matrix_p;
static GLfloat motion_type;

#include "../ui_statics.h"

static void init_background(UIBackground * background) {
    GLuint * vertex_indexes = background->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
        
    Vec4 * vertexes_attributes = background->vertexes_attributes;
    vertexes_attributes[0].x = -1.0f;
    vertexes_attributes[0].y =  1.0f;
    vertexes_attributes[1].x = -1.0f;
    vertexes_attributes[1].y = -1.0f;
    vertexes_attributes[2].x =  1.0f;
    vertexes_attributes[2].y =  1.0f;
    vertexes_attributes[3].x =  1.0f;
    vertexes_attributes[3].y = -1.0f;
    
    DSTUDIO_SET_S_T_COORDINATES
    
    background->index_buffer_object = 0;
    glGenBuffers(1, &background->index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, background->index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   
    GLuint * vertex_buffer_object_p = &background->vertex_buffer_object;
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    get_png_pixel(DSANDGRAINS_BACKGROUND_ASSET_PATH, &background->texture, PNG_FORMAT_RGB);
    
    glGenTextures(1, &background->texture_id);
    glBindTexture(GL_TEXTURE_2D, background->texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, background->texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &background->vertex_array_object);
    glBindVertexArray(background->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, background->vertex_buffer_object);         
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void render_background(UIBackground * background) {
        glBindTexture(GL_TEXTURE_2D, background->texture_id);
            glBindVertexArray(background->vertex_array_object);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, background->index_buffer_object);
                    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid *) 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
}

static void render_viewport() {
    glUseProgram(non_interactive_program_id);
        render_background(background_p);
        
    glUseProgram(interactive_program_id);
        // KNOBS
        motion_type = 0.0;
        glUniform1f(motion_type_id, motion_type);

        glUniformMatrix2fv(scale_matrix_id, 1, GL_FALSE, sample_knobs_scale_matrix_p);
        render_knobs(sample_knobs_p);
        
        glUniformMatrix2fv(scale_matrix_id, 1, GL_FALSE, sample_small_knobs_scale_matrix_p);
        render_knobs(sample_small_knobs_p);
        
        glUniformMatrix2fv(scale_matrix_id, 1, GL_FALSE, voice_knobs_scale_matrix_p);
        render_knobs(voice_knobs_p);

        // SLIDERS
        motion_type = 1.0;
        glUniform1f(motion_type_id, motion_type);

        glUniformMatrix2fv(scale_matrix_id, 1, GL_FALSE, sliders_dahdsr_scale_matrix_p);
        render_sliders(sliders_dahdsr_p);
}

// Should be splitted
void * ui_thread(void * arg) {
<<<<<<< HEAD
    int fresh_window_attrib;
=======
    int fresh_window_attrib;
>>>>>>> 350f630fe4eaa14f1056d0906886b8cf24f6ba68
    UI * ui = arg;
    background_p = &ui->background;
    sample_knobs_p = &ui->sample_knobs;
    sample_small_knobs_p = &ui->sample_small_knobs;
    sliders_dahdsr_p = &ui->sliders_dahdsr;
    voice_knobs_p = &ui->voice_knobs;
    ui_areas = &ui->areas[0];
    ui_callbacks = &ui->callbacks[0];

    DSTUDIO_EXIT_IF_FAILURE( (glfwInit() != GLFW_TRUE) )
    
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT, "DSANDGRAINS", NULL, NULL);
    
    DSTUDIO_EXIT_IF_FAILURE_GLFW_TERMINATE((window == 0))
        
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // Window shouldn't be resized, but with some windows manager it might happens. OpenGL need to be notified to redraw the whole scene.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_change_callback);   
    	
    DSTUDIO_EXIT_IF_FAILURE_GLFW_TERMINATE(load_extensions())
    
    create_shader_program(&interactive_program_id, &non_interactive_program_id);
    init_background(background_p);
    
    init_knobs_gpu_side(sample_knobs_p);
    init_knobs_gpu_side(sample_small_knobs_p);
    init_knobs_gpu_side(voice_knobs_p);
    init_sliders_gpu_side(sliders_dahdsr_p);
    
    finalize_knobs(sample_knobs_p);
    finalize_knobs(sample_small_knobs_p);
    finalize_knobs(voice_knobs_p);
    finalize_sliders(sliders_dahdsr_p);

    scale_matrix_id = glGetUniformLocation(interactive_program_id, "scale_matrix");
    motion_type_id = glGetUniformLocation(interactive_program_id, "motion_type");
    sample_knobs_scale_matrix_p = &sample_knobs_p->scale_matrix[0].x;
    sample_small_knobs_scale_matrix_p = &sample_small_knobs_p->scale_matrix[0].x;
    sliders_dahdsr_scale_matrix_p = &sliders_dahdsr_p->scale_matrix[0].x;
    voice_knobs_scale_matrix_p = &voice_knobs_p->scale_matrix[0].x;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    
    while (!glfwWindowShouldClose(window)) {
        usleep(framerate);
        
        fresh_window_attrib = glfwGetWindowAttrib(window, GLFW_VISIBLE);
        if (!window_visible && fresh_window_attrib) {
            first_render = 1;
        }
        window_visible = fresh_window_attrib;
<<<<<<< HEAD
=======
>>>>>>> 350f630fe4eaa14f1056d0906886b8cf24f6ba68
        if (first_render) {
            glScissor(0, 0, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
            first_render = 0;
            render_viewport();
        }
        else if (areas_index >= 0) {
            glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
            render_viewport();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return NULL;
}
