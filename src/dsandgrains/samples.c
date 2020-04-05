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

#include "../instances.h"
#include "samples.h"

SampleContext * g_current_active_sample = 0;
ThreadControl g_samples_thread_control = {0};
UIInteractiveList g_ui_samples = {0};

void bind_samples_interactive_list(UIElements * line) {
    g_ui_samples.update_request = -1;
    if (line == NULL) {
        line = g_ui_samples.lines;
        g_ui_samples.window_offset = 0;
        update_current_voice(0);
    }
    Samples * samples = g_current_active_voice->sub_contexts;
    g_ui_samples.source_data = (char*) samples->contexts;
    g_ui_samples.source_data_count = &samples->count;
    select_item(
        line,
        DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
    );
}

void init_samples_interactive_list(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    g_samples_thread_control.shared_mutex = &g_instances.thread_control.mutex;

    init_interactive_list(
        &g_ui_samples,
        ui_elements,
        lines_number,
        string_size,
        sizeof(VoiceContext),
        &samples->count,
        (char *) samples->contexts,
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

void update_samples_ui_list() {
    update_insteractive_list(&g_ui_samples);
}
