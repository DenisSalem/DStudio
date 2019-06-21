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

static inline float compute_knob_rotation(double xpos, double ypos, Vec2 active_knob_center) {
    float rotation = 0;

    float y = - ypos + active_knob_center.y;
    float x = xpos - active_knob_center.x;
    rotation = -atan(x / y);

    if (y < 0) {
        if (x < 0) {
            rotation += 3.141592;
        }
        else {
                rotation -= 3.141592;
        }
    }
    if (rotation > 2.356194) {
        rotation = 2.356194;
    }
    else if (rotation < -2.356194) {
        rotation = -2.356194;
    }
    return rotation;
}

#define DEFINE_CURSOR_POSITION_CALLBACK \
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){ \
    float rotation; \
    if (active_ui_element.callback == NULL) { \
        return; \
    } \
    if (active_ui_element.type == DSTUDIO_KNOB_TYPE_1) { \
        rotation = compute_knob_rotation(xpos, ypos, active_knob_center); \
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &rotation); \
    } \
    else if (active_ui_element.type == DSTUDIO_KNOB_TYPE_2) { \
        rotation = compute_knob_rotation(xpos, ypos, active_knob_center); \
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &rotation); \
    } \
}

#define DEFINE_FRAMEBUFFER_SIZE_CHANGE_CALLBACK \
static void framebuffer_size_change_callback(GLFWwindow * window, int width, int height) { \
    first_render = 1; \
}
    
#define DEFINE_MOUSE_BUTTON_CALLBACK \
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) { \
    double xpos, ypos; \
    glfwGetCursorPos(window, &xpos, &ypos); \
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) { \
        for (char i = 0; i < DSANDGRAINS_UI_ELEMENTS_COUNT; i++) { \
            if (xpos > ui_areas[i].min_x && xpos < ui_areas[i].max_x && ypos > ui_areas[i].min_y && ypos < ui_areas[i].max_y) { \
                active_ui_element.callback = ui_callbacks[i].callback; \
                active_ui_element.index = ui_callbacks[i].index; \
                active_ui_element.context_p = ui_callbacks[i].context_p; \
                active_ui_element.type = ui_callbacks[i].type; \
                /* SETUP glScissor params */ \
                if (areas_index < 0) { \
                    areas_index = i; \
                    scissor_x = (GLint) ui_areas[i].min_x; \
                    scissor_y = (GLint) DSANDGRAINS_VIEWPORT_HEIGHT - ui_areas[i].max_y; \
                    scissor_width = (GLsizei) ui_areas[i].max_x - ui_areas[i].min_x; \
                    scissor_height = (GLsizei) ui_areas[i].max_y - ui_areas[i].min_y; \
                } \
                if (active_ui_element.type & 3) {/* IF DSTUDIO_KNOB_TYPE_1 OR DSTUDIO_KNOB_TYPE_2 */ \
                    active_knob_center.x = ui_areas[i].x; \
                    active_knob_center.y = ui_areas[i].y; \
                } \
                break; \
            } \
        } \
    } \
    if (action == GLFW_RELEASE) { \
        active_ui_element.callback = NULL; \
        areas_index = -1; \
    } \
}
