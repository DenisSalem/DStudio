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

#include "bar_plot.h"

void update_bar_plot_as_waveform(UIElements * bar_plot, SharedSample * shared_sample) {
    unsigned int sub_sample_size = 0;
    
    float positive_average = 0;
    float positive_count = 0;
    float negative_average = 0;
    float negative_count = 0;
    
    if (shared_sample == NULL) {
        explicit_bzero(bar_plot->instance_motions_buffer, sizeof(GLfloat) * bar_plot->count);
        bar_plot->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
        return;
    }
    
    float * left = shared_sample->left;
    float * right = shared_sample->right;
    
    unsigned int bar_index = 0;
    
    DSTUDIO_TRACE_ARGS("%lu %d", shared_sample->size, bar_plot->count);
    
    if (shared_sample->size > bar_plot->count) {
        DSTUDIO_TRACE
        sub_sample_size = shared_sample->size / bar_plot->count;
    
        for (unsigned int i = 0; i < shared_sample->size; i++) {
            if ((i != 0) && (i % sub_sample_size == 0)) {
                // TODO: NEED TO BE NORMALIZED
                bar_plot->instance_motions_buffer[bar_index++] = ((positive_average / positive_count) -  (negative_average / negative_count)) / 2.0;
                positive_average = 0;
                positive_count = 0;
                negative_average = 0;
                negative_count = 0;

            }
            
            if (left && left[i] >= 0) {
                positive_average += left[i];
                positive_count++;
            }
            else if (left && left[i] < 0) {
                negative_average += left[i];
                negative_count++;
            }
            if (right && right[i] >= 0) {
                positive_average += right[i];
                positive_count++;
            }
            else if (right && right[i] < 0) {
                negative_average += right[i];
                negative_count++;
            }
        }
        DSTUDIO_TRACE
        
        bar_plot->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
    }
}
