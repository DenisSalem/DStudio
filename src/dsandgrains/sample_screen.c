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

#include "../bar_plot.h"
#include "sample_screen.h"
#include "samples.h"
#include "ui.h"

static unsigned char s_update_request = 0;
static SharedSample * s_shared_sample = 0;

void bind_new_data_to_sample_screen(SharedSample * shared_sample) {
    s_update_request |= DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_DATA | DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_RANGE;
    s_shared_sample = shared_sample;
}

void update_sample_screen() {
    if (s_update_request & DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_DATA) {
        update_bar_plot_as_waveform(&g_ui_elements_struct.sample_screen, s_shared_sample, DSTUDIO_DO_ANIMATE);
    }
    if (s_update_request & DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_RANGE) {
        update_sample_screen_range();
    }
    s_update_request = 0;
}

void request_sample_screen_range_update(UIElements * ui_element) {
    (void) ui_element;
    s_update_request |= DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_RANGE;
}

void update_sample_screen_range() {
    if (g_current_active_sample == NULL) {
        return;
    }
    float sample_start = g_current_active_sample->start.computed;
    float sample_end = g_current_active_sample->end.computed; 
    
    float ui_start = sample_start < sample_end ? sample_start : sample_end;
    float ui_end = sample_start < sample_end ? sample_end : sample_start;
    GLfloat * alphas_buffer = g_ui_elements_struct.sample_screen.instance_alphas_buffer;

    for (unsigned int i = 0; i < (unsigned int) (ui_start * g_ui_elements_struct.sample_screen.count); i++) {
        alphas_buffer[i] = 0.5;
    }

    for (unsigned int i = (unsigned int) (ui_start * g_ui_elements_struct.sample_screen.count); i < (unsigned int ) (ui_end * g_ui_elements_struct.sample_screen.count); i++) {
        alphas_buffer[i] = 0.75;
    }

    for (unsigned int i = (unsigned int ) (ui_end * g_ui_elements_struct.sample_screen.count); i < g_ui_elements_struct.sample_screen.count; i++) {
        alphas_buffer[i] = 0.5;
    }
    
    g_ui_elements_struct.sample_screen.render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
}
