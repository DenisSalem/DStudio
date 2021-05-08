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

static void process_channel(SampleContext * sample, float * channel_in, float * channel_out, unsigned int frame_size) {
    long unsigned int sample_size = sample->shared_sample.size;
    float sample_amount = sample->amount.computed <= 1.0 ? sample->amount.computed : 1 + (sample->amount.computed-1) * 10; 
    float sample_stretch = sample->stretch.computed <= 1.0 ? 0.5 + 0.5*sample->stretch.computed : sample->stretch.computed; 
    
    (void) sample_stretch;
    
    long unsigned int processed_sub_sample_count = sample->processed_sub_sample_count;
    
    for (unsigned int i = 0; i < frame_size; i++) {
        channel_out[i] += sample_amount * channel_in[(processed_sub_sample_count+i) % sample_size];
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

        sample->processed_sub_sample_count = sample->processed_sub_sample_count + frame_size;
    }
}

