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

static float        s_stretch_buffer[8192] = {0};
static float s_stretch_buffer_indexes[8192] = {0};

static void process_channel(SampleContext * sample, float * channel_in, float * channel_out, unsigned int frame_size) {
    long unsigned int sample_size = sample->shared_sample.size;
    float sample_amount = sample->amount.computed <= 1.0 ? sample->amount.computed : 1 + (sample->amount.computed-1) * 10; 
    float sample_stretch = sample->stretch.computed <= 1.0 ? 0.5 + 0.5*sample->stretch.computed : sample->stretch.computed; 
    long unsigned int processed_sub_sample_count = sample->processed_sub_sample_count;
    unsigned int stretched_size = frame_size / sample_stretch;

    (void) s_stretch_buffer_indexes;
    
    if (sample_stretch > 1.0) { // Require interpollation
        for (unsigned int i = 0; i < frame_size; i++) {
            channel_out[i] += sample_amount * channel_in[(processed_sub_sample_count+i) % sample_size];
        }
    }
    else { // Average
        unsigned previous_index = -1;
        unsigned current_index = 0;
        float sample_count = 0;
        float current_sample = 0;
        for (unsigned int i = 0; i < stretched_size; i++) {
                current_index = (unsigned) (((float) i / (float) stretched_size) * (float) frame_size);
                if (current_index != previous_index) {
                    if (sample_count) {
                        s_stretch_buffer[current_index] = current_sample / (float) sample_count; 
                    }
                    sample_count = 1;
                    current_sample = channel_in[(processed_sub_sample_count+i) % sample_size];
                }
                else {
                    sample_count+=1;
                    current_sample += channel_in[(processed_sub_sample_count+i) % sample_size];
                }
        }
        channel_in = &s_stretch_buffer[0];
    }
    
    for (unsigned int i = 0; i < frame_size; i++) {
        channel_out[i] += sample_amount * channel_in[i];
    }
}

void dsandgrains_audio_process(VoiceContext * voice, float * out_left, float * out_right, unsigned int frame_size) {
    Samples * samples = voice->sub_contexts;
    SampleContext * sample = 0;
    
    for (unsigned int sample_index = 0; sample_index < samples->count; sample_index++) {
        sample = &samples->contexts[sample_index];

        if (out_left) {
            process_channel(sample, sample->shared_sample.left, out_left, frame_size);
        }
        if (out_right) {
            process_channel(sample, sample->shared_sample.right, out_right, frame_size);
        }

        sample->processed_sub_sample_count = (long unsigned)(((float)frame_size/sample->stretch.computed) + sample->processed_sub_sample_count) % sample->shared_sample.size;
    }
}

