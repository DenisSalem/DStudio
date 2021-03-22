/*
 * Copyright 2019, 2021 Denis Salem
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

#include "sliders.h"

inline float compute_slider_percentage_value(int ypos) {
    if (ypos > g_active_slider_range_max) {
        ypos = g_active_slider_range_max;
    }
    else if (ypos < g_active_slider_range_min) {
        ypos = g_active_slider_range_min;
    }
    
    return 1.0 - (float) (ypos - g_active_slider_range_min) / (float) (g_active_slider_range_max - g_active_slider_range_min);
}

inline float compute_slider_translation(int ypos) {
    if (ypos > g_active_slider_range_max) {
        ypos = g_active_slider_range_max;
    }
    else if (ypos < g_active_slider_range_min) {
        ypos = g_active_slider_range_min;
    }
    float translation = - ypos + g_ui_element_center_y;
    return translation / (g_previous_window_scale.height >> 1);
}

void compute_slider_scissor_y(UIElements * ui_elements) {
    ui_elements->coordinates_settings.scissor.y = (\
        1.0+ \
        ui_elements->coordinates_settings.instance_offsets_buffer->y+ \
        ui_elements->instance_motions_buffer[0] \
        - (ui_elements->coordinates_settings.scale_matrix[1].y) \
    ) * (g_previous_window_scale.height >> 1);
    
    ui_elements->coordinates_settings.scissor.height = ui_elements->coordinates_settings.scale_matrix[1].y * g_previous_window_scale.height;
}

void compute_slider_in_motion_scissor_y(UIElements * slider, GLfloat motion) {
    GLint current_scissor_y;
    GLint previous_scissor_y;
    GLint new_scissor_y;
    GLsizei height;
    
    current_scissor_y = (\
        DSTUDIO_SLIDER_SCISSOR_SAFE_OFFSET+ \
        slider->coordinates_settings.instance_offsets_buffer->y+ \
        motion \
        - (slider->coordinates_settings.scale_matrix[1].y) \
    ) * (g_previous_window_scale.height >> 1);

    previous_scissor_y = slider->render_state ? g_saved_scissor_y : slider->coordinates_settings.scissor.y;
    new_scissor_y = (previous_scissor_y < current_scissor_y ? previous_scissor_y : current_scissor_y);
    height = previous_scissor_y < current_scissor_y ? current_scissor_y - previous_scissor_y : previous_scissor_y - current_scissor_y;
    
    slider->coordinates_settings.scissor.y = new_scissor_y;

    if (slider->render_state) {
        slider->coordinates_settings.scissor.height += height;
    } else {
        g_saved_scissor_y = new_scissor_y;
        slider->coordinates_settings.scissor.height = height + slider->coordinates_settings.scale_matrix[1].y * (g_previous_window_scale.height);
    }
}
