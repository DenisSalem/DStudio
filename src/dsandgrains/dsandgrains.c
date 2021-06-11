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

#include "dsandgrains.h"

static long int s_start = 0;
static long int s_end = 0;
static int s_direction = 1;
static long int s_range = 0;

static void process_channel(SampleContext * sample, float * channel_in, float * channel_out, unsigned int frame_size) {
    float sample_amount = sample->amount.computed <= 1.0 ? sample->amount.computed : 1 + (sample->amount.computed-1) * 10; 
    float sample_stretch = sample->stretch.computed <= 1.0 ? 0.5 + 0.5*sample->stretch.computed : sample->stretch.computed; 
    
    (void) sample_stretch;
    
    long int processed_index = sample->processed_index;
    long int index = 0;
    
    for (int i = 0; i < (int) frame_size; i++) {
        // TODO : Could be optimized and cleaned
        
        if (s_direction > 0) {
            index = processed_index + i;
            if (index >= s_end) {
                index -= s_range;
            }
        }
        else {
            index = processed_index - i;
            if (index < s_end) {
                index +=(s_range);
            }
        }
        
        channel_out[i] += sample_amount * channel_in[index];
    }
}

void dsandgrains_audio_process(VoiceContext * voice, float * out_left, float * out_right, uint_fast32_t frame_size) {
    Samples * samples = voice->sub_contexts;
    SampleContext * sample = 0;

    for (unsigned int sample_index = 0; sample_index < samples->count; sample_index++) {
        sample = &samples->contexts[sample_index];
        
        s_start =  sample->start.computed * sample->shared_sample.size;
        s_end =  sample->end.computed * sample->shared_sample.size;

        if (s_start < s_end) {
            s_direction = 1;
            s_range = s_end - s_start;
        }
        else if(s_start > s_end) {
            s_direction = -1;
            s_range = s_start - s_end;
        }
        else {
            continue;
        }
        
        if (s_direction > 0 && (sample->processed_index < s_start || sample->processed_index >= s_end)) {
            sample->processed_index = s_start;    
        }
        if (s_direction < 0 && (sample->processed_index < s_end || sample->processed_index > s_start)) {
            sample->processed_index = s_start;    
        }

        if (out_left) {
            process_channel(sample, sample->shared_sample.left, out_left, frame_size);
        }
        if (out_right) {
            process_channel(sample, sample->shared_sample.right, out_right, frame_size);
        }

        if (s_direction > 0) {
            sample->processed_index = sample->processed_index + frame_size;
            if (sample->processed_index >= s_end) {
                sample->processed_index -= s_range;
            }
        }
        else {
            sample->processed_index = sample->processed_index - frame_size;
            if (sample->processed_index < s_end) {
                sample->processed_index += s_range;
            }
        }
    }
}

