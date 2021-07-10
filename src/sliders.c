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

#include "dstudio.h"

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
    return translation / (g_dstudio_viewport_height >> 1);
}

void compute_slider_scissor_y(UIElements * slider) {
    slider->coordinates_settings.scissor.y = -2 + (\
        (1 + \
        slider->coordinates_settings.instance_offsets_buffer->y + \
        *slider->instance_motions_buffer \
        - (slider->coordinates_settings.scale_matrix[1].y) \
    ) * (g_dstudio_viewport_height >> 1));

    slider->coordinates_settings.scissor.height = 4 + \
        slider->coordinates_settings.scale_matrix[1].y * \
        g_dstudio_viewport_height;
}

void compute_slider_in_motion_scissor_y(UIElements * slider) {
    GLint local_scissor_y = -2.0 + ( \
        (1.0 + \
        slider->coordinates_settings.instance_offsets_buffer->y + \
        *slider->instance_motions_buffer - \
        slider->coordinates_settings.scale_matrix[1].y
    ) * (g_dstudio_viewport_height >> 1));

    slider->coordinates_settings.scissor.y = local_scissor_y > slider->coordinates_settings.previous_scissor.y ? slider->coordinates_settings.previous_scissor.y : local_scissor_y;

    slider->coordinates_settings.scissor.height = \
        local_scissor_y > slider->coordinates_settings.previous_scissor.y ? \
            local_scissor_y - slider->coordinates_settings.previous_scissor.y : \
            slider->coordinates_settings.previous_scissor.y - local_scissor_y;
            
    slider->coordinates_settings.scissor.height += \
        4 + \
        slider->coordinates_settings.scale_matrix[1].y * \
        (g_dstudio_viewport_height);
}
