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
