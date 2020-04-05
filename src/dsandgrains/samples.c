/*
 * Copyright 2019, 2020 Denis Salem
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

#include "samples.h"

SampleContext * g_current_active_sample = 0;
ThreadControl g_samples_thread_control = {0};
UIInteractiveList g_ui_samples = {0};

void init_samples_interactive_list(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    printf("contexts: %lu", (unsigned long) samples->contexts);
    init_interactive_list(
        &g_ui_voices,
        ui_elements,
        lines_number,
        string_size,
        sizeof(VoiceContext),
        &samples->count,
        samples->contexts->name,
        &g_samples_thread_control,
        select_voice_from_list,
        1,
        item_offset_y
    );
}

unsigned int select_sample_from_list(
    unsigned int index
) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    if (index != samples->index && index < samples->count) {
        update_current_sample(index);
        return 1;
    }
    return 0;
}

// TODO: the following could be generalized and not implemented by consumer.
void update_current_sample(unsigned int index) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    samples->index = index;
    g_current_active_sample = &samples->contexts[index];
}
