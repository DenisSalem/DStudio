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

void dsandgrains_audio_process(VoiceContext * voice, float * out_left, float * out_right, unsigned int frame_size) {
    SampleContext * sample = 0;
    float * sample_left = 0;
    float * sample_right = 0;
    Samples * samples = voice->sub_contexts;
    for (unsigned int sample_index = 0; sample_index < samples->count; sample_index++) {
        sample = &samples->contexts[sample_index];
        sample_left = sample->shared_sample.left;
        sample_right = sample->shared_sample.channels == 2 ? sample->shared_sample.right : sample_left;
        for (unsigned int i = 0; i < frame_size; i++) {
            out_left[i] += sample_left[(sample->processed_sub_sample_count+i) % sample->shared_sample.size];
            out_right[i] += sample_right[(sample->processed_sub_sample_count+i) % sample->shared_sample.size];
        }
        sample->processed_sub_sample_count+=frame_size;
    }
}

