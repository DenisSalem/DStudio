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

static Vec2 active_slider_range = {0};
static UICallback active_ui_element = {0};
static Vec2 active_ui_element_center = {0};
static int render_mask = 0;
static int ui_element_index = 0;
static int mouse_state = 0;
static double list_item_click_timestamp = 0;

static inline float compute_knob_rotation(int xpos, int ypos) {
    render_mask = DSTUDIO_RENDER_KNOBS;
    float rotation = 0;

    float y = - ypos + active_ui_element_center.y;
    float x = xpos - active_ui_element_center.x;
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

static inline float compute_slider_translation(int ypos) {
    render_mask = DSTUDIO_RENDER_SLIDERS;
    if (ypos > active_slider_range.y) {
        ypos = active_slider_range.y;
    }
    else if (ypos < active_slider_range.x) {
        ypos = active_slider_range.x;
    }
    float translation = - ypos + active_ui_element_center.y;
    return translation / (DSTUDIO_VIEWPORT_HEIGHT >> 1);
}

static void cursor_position_callback(int xpos, int ypos){
    float motion;
    if (active_ui_element.callback == NULL) {
        return;
    }
    if (active_ui_element.type == DSTUDIO_KNOB_TYPE_CONTINUE) {
        motion = compute_knob_rotation(xpos, ypos);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &motion);
    }
    else if (active_ui_element.type == DSTUDIO_KNOB_TYPE_DISCRETE) {
        motion = compute_knob_rotation(xpos, ypos);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &motion);
    }
    else if (active_ui_element.type == DSTUDIO_SLIDER_TYPE_VERTICAL) {
        #ifdef DSTUDIO_DEBUG
        printf("Update Slider\n");
        #endif
        motion = compute_slider_translation(ypos);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &motion);
    }
}
    
static void mouse_button_callback(int xpos, int ypos, int button, int action) {
    double timestamp = 0;
    if (button == DSTUDIO_MOUSE_BUTTON_LEFT && action == DSTUDIO_MOUSE_BUTTON_PRESS) {
        mouse_state = 1;
        for (int i = 0; i < DSANDGRAINS_UI_ELEMENTS_COUNT; i++) {
            ui_element_index = i;
            if (xpos > ui_areas[i].min_x && xpos < ui_areas[i].max_x && ypos > ui_areas[i].min_y && ypos < ui_areas[i].max_y) {
                if (ui_callbacks[i].type == DSTUDIO_BUTTON_TYPE_REBOUNCE) {
                    button_settings_array[i].application_callback(&button_settings_array[i].flags);
                    ui_callbacks[i].callback(0, ui_callbacks[i].context_p, &button_settings_array[i]);
                    break;
                }
                if (ui_callbacks[i].type == DSTUDIO_BUTTON_TYPE_LIST_ITEM) {
                    timestamp = get_timestamp();
                    if (timestamp - list_item_click_timestamp < DSTUDIO_DOUBLE_CLICK_DELAY) {
                        update_text_pointer_context(
                            DSTUDIO_BUTTON_TYPE_LIST_ITEM,
                            button_settings_array[i].index,
                            button_settings_array[i].context
                        );
                    }
                    else {
                        clear_text_pointer();
                        button_settings_array[i].context.interactive_list->application_callback(
                            button_settings_array[i].index
                        );
                    }
                    list_item_click_timestamp = timestamp;
                    break;
                }
                active_ui_element.callback = ui_callbacks[i].callback;
                active_ui_element.index = ui_callbacks[i].index;
                active_ui_element.context_p = ui_callbacks[i].context_p;
                active_ui_element.type = ui_callbacks[i].type;
                /* SETUP glScissor params */
                if (areas_index < 0) {
                    areas_index = i;
                    SET_SCISSOR
                }
                
                active_ui_element_center.x = ui_areas[i].x;
                active_ui_element_center.y = ui_areas[i].y;
                
                if (active_ui_element.type & 4) { /* IF DSTUDIO_SLIDER_TYPE_1 */
                    active_slider_range.x = ui_areas[i].min_y + DSANDGRAINS_SLIDER1_SCALE / 2;
                    active_slider_range.y = ui_areas[i].max_y - DSANDGRAINS_SLIDER1_SCALE / 2;
                }
                break;
            }
        }
    }
    else if (action == DSTUDIO_MOUSE_BUTTON_RELEASE) {
        mouse_state = 0;
        active_ui_element.callback = NULL;
        areas_index = -1;
        render_mask = 0;
    }
}
