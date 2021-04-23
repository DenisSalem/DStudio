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

void update_bar_plot_as_waveform(UIElements * bar_plot, SharedSample * shared_sample, unsigned int animate) {
    unsigned int sub_sample_size = 0;
    
    float period_extremum = 0;
    float period_minimum = 0;
    float peak = 0;
    float local_peak = 0;
    GLfloat * motions_buffer = 0;
    GLfloat * offsets_buffer_in = 0;
    Vec4 * offsets_buffer_out = 0;
    
    if (shared_sample == NULL) {
        if (animate) {
            motions_buffer = dstudio_alloc(sizeof(GLfloat) * bar_plot->count, DSTUDIO_FAILURE_IS_FATAL);
            animate_motions_transitions(motions_buffer, bar_plot);
            dstudio_free(motions_buffer);
        }
        else {
            explicit_bzero(bar_plot->instance_motions_buffer, sizeof(GLfloat) * bar_plot->count);
        }
        bar_plot->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
        return;
    }
    
    float * left = shared_sample->left;
    float * right = shared_sample->right;
    
    unsigned int bar_index = 0;
        
    if (shared_sample->size > bar_plot->count) {
        sub_sample_size = shared_sample->size / bar_plot->count;
        motions_buffer = animate ? dstudio_alloc(sizeof(GLfloat) * bar_plot->count, DSTUDIO_FAILURE_IS_FATAL) : bar_plot->instance_motions_buffer;
        offsets_buffer_out = animate ? dstudio_alloc(sizeof(Vec4) * bar_plot->count, DSTUDIO_FAILURE_IS_FATAL) : bar_plot->coordinates_settings.instance_offsets_buffer;
        offsets_buffer_in = dstudio_alloc(sizeof(GLfloat) * bar_plot->count, DSTUDIO_FAILURE_IS_FATAL);
        
        for (unsigned int i = 0; i < shared_sample->size; i++) {
            if ((i != 0) && (i % sub_sample_size == 0)) {
                local_peak = period_extremum > ABSOLUTE_VALUE(period_minimum) ? period_extremum : period_minimum;
                if (local_peak > peak) {
                    peak = local_peak;
                }
                motions_buffer[bar_index] = period_extremum - period_minimum;
                offsets_buffer_in[bar_index++] = (period_extremum + period_minimum)/2.0;
                if (bar_index == bar_plot->count) {
                    break;
                }
                period_extremum = -1.0;
                period_minimum = 1.0;
            }
            
            if (left) {
                if(left[i] > period_extremum) {
                    period_extremum = left[i];
                }
                if(left[i] < period_minimum) {
                    period_minimum = left[i];
                }
            }

            if (right) {
                if (right[i] > period_extremum) {
                    period_extremum = right[i];
                }
                if (right[i] < period_minimum) {
                    period_minimum = right[i];
                }
            }
        }
        // Normalize and apply offset
        float multiplier = bar_plot->coordinates_settings.scale_matrix[1].y * (1.0/peak);
        peak*=2;
        for (unsigned int i = 0; i < bar_plot->count; i++) {
            motions_buffer[i] /= peak;
            offsets_buffer_out[i].w = offsets_buffer_in[i] * multiplier;
        }

        if (animate) {
            animate_motions_transitions(motions_buffer, bar_plot);
            animate_offsets_transitions(offsets_buffer_out, bar_plot, DSTUDIO_OFFSET_W_TRANSITION);
            dstudio_free(motions_buffer);
            dstudio_free(offsets_buffer_out);
        }

        dstudio_free(offsets_buffer_in);

        bar_plot->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
    }
}
