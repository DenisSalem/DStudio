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
static UIBackground * background_p;
static UIArea * ui_areas;
static UICallback * ui_callbacks;
static UICallback active_ui_element = {0};
static Vec2 active_knob_center;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
    float rotation;
    if (active_ui_element.callback == NULL) {
        return;
    }
    
    if (active_ui_element.type == DSTUDIO_KNOB_TYPE) {
        rotation = compute_knob_rotation(xpos, ypos, active_knob_center);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &rotation);
    }
    
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        for (int i = 0; i < DSANDGRAINS_UI_ELEMENTS_COUNT; i++) {
            if (xpos > ui_areas[i].min_x && xpos < ui_areas[i].max_x && ypos > ui_areas[i].min_y && ypos < ui_areas[i].max_y) {
                active_ui_element.callback = ui_callbacks[i].callback;
                active_ui_element.index = ui_callbacks[i].index;
                active_ui_element.context_p = ui_callbacks[i].context_p;
                active_ui_element.type = ui_callbacks[i].type;
                if (active_ui_element.type == DSTUDIO_KNOB_TYPE) {
                    active_knob_center.x = ui_areas[i].x;
                    active_knob_center.y = ui_areas[i].y;
                }
                break;
            }
        }
    }
    if (action == GLFW_RELEASE) {
        active_ui_element.callback = NULL;
    }
}

// Should be splitted
void * ui_thread(void * arg) {
    UI * ui = arg;
    background_p = &ui->background;
    sample_knobs_p = &ui->sample_knobs;
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

    DSTUDIO_EXIT_IF_FAILURE_GLFW_TERMINATE(load_extensions())

    GLuint interactive_program_id, non_interactive_program_id;
    create_shader_program(&interactive_program_id, &non_interactive_program_id);

    init_background(background_p);
    
    init_knobs_gpu_side(sample_knobs_p);
    
    finalize_knobs(sample_knobs_p, interactive_program_id);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    while (!glfwWindowShouldClose(window)) {
        usleep(20000);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(non_interactive_program_id);
            render_background(background_p);

        glUseProgram(interactive_program_id);
            render_knobs(sample_knobs_p);
            
        glUseProgram(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(non_interactive_program_id);
    glDeleteProgram(interactive_program_id);
    glfwTerminate();
    free_knobs(sample_knobs_p);
    free_background(background_p);
    return NULL;
}

static void free_background(UIBackground * background) {
    glDeleteBuffers(1, &background->index_buffer_object);
    glDeleteBuffers(1, &background->vertex_buffer_object);
    glDeleteVertexArrays(1, &background->vertex_array_object);
    glDeleteTextures(1, &background->texture_id);
    free(background->texture);
}

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
    
    get_png_pixel("../assets/dsandgrains_background.png", &background->texture, 0);
    
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
