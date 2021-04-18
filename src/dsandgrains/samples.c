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

#include "../instances.h"
#include "samples.h"
#include "sample_screen.h"

SampleContext * g_current_active_sample = 0;
UIInteractiveList g_ui_samples = {0};

void bind_samples_interactive_list(UIElements * line, ListItemOpt flag) {
    g_ui_samples.update_request = -1;
    if (line == NULL) {
        line = g_ui_samples.lines;
        g_ui_samples.window_offset = 0;
        update_current_sample(0);
    }
    Samples * samples = g_current_active_voice->sub_contexts;
    g_ui_samples.source_data = (char*) samples->contexts;
    g_ui_samples.source_data_count = &samples->count;
    select_item(
        line,
        flag
    );
}

void init_samples_interactive_list(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;

    init_interactive_list(
        &g_ui_samples,
        ui_elements,
        lines_number,
        string_size,
        sizeof(SampleContext),
        &samples->count,
        (char *) samples->contexts,
        select_sample_from_list,
        DSTUDIO_NO_CALLBACK,
        1,
        item_offset_y
    );
    #ifdef DSTUDIO_DEBUG
    strcat(g_ui_samples.trace, "g_ui_samples");
    #endif
}

UIElements * new_sample(char * filename, SharedSample shared_sample) {
    UIElements * line = 0;
    Samples * samples = g_current_active_voice->sub_contexts;
    SampleContext * new_sample_context = dstudio_realloc(
        samples->contexts,
        (samples->count + 1) * sizeof(SampleContext)
    );
    if (new_sample_context == NULL) {
        // TODO: LOG FAILURE
        printf("Failed to import sample\n");
        return 0;
    }
    explicit_bzero(
        &new_sample_context[samples->count],
        sizeof(SampleContext)
    );

    memcpy(&new_sample_context[samples->count].shared_sample, &shared_sample, sizeof(SharedSample));

    samples->index = samples->count++;
    samples->contexts = new_sample_context;
    g_current_active_sample = &samples->contexts[samples->index];
    
    strncpy(g_current_active_sample->name, filename, g_ui_samples.string_size-1);
    #ifdef DSTUDIO_DEBUG
    printf(
        "%s %s %s\n",
        g_current_active_instance->name,
        g_current_active_voice->name,
        g_current_active_sample->name
    );
    #endif

    if (samples->count > g_ui_samples.lines_number) {
        g_ui_samples.window_offset = samples->count - g_ui_samples.lines_number;
        g_ui_samples.update_request = -1;
    }
    else {
        g_ui_samples.update_request = samples->index;
        g_ui_samples.window_offset = 0;
    }

    line = &g_ui_samples.lines[samples->index-g_ui_samples.window_offset];
    bind_samples_interactive_list(line, DSTUDIO_SELECT_ITEM_WITH_CALLBACK);
    
    return line;
}

unsigned int select_sample_from_list(
    unsigned int index
) {
    unsigned int ret = 0;
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    DSTUDIO_TRACE_ARGS("%d %d", index != samples->index, index < samples->count);
    
    if (samples->count && index < samples->count) {
        update_current_sample(index);
        ret = 1;
    }
    
    bind_new_data_to_sample_screen(samples->count ? &g_current_active_sample->shared_sample : 0);

    return ret;
}

UIElements * set_samples_list_from_parent() {
    UIElements * line;
    unsigned int sample_index;
    Samples * samples = g_current_active_voice->sub_contexts;
    sample_index = samples->index;
    if (sample_index < g_ui_samples.lines_number) {
        line = &g_ui_samples.lines[sample_index];
        g_ui_samples.window_offset = 0;
    }
    else {
        line = &g_ui_samples.lines[g_ui_voices.lines_number-1];
        g_ui_samples.window_offset = sample_index - g_ui_samples.lines_number + 1;
    }
    return line;
}

// TODO: the following could be generalized and not implemented by consumer.
void update_current_sample(unsigned int index) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    samples->index = index;
    g_current_active_sample = &samples->contexts[index];
}

void update_samples_ui_list() {
    if (g_ui_samples.update_request) {
        update_insteractive_list(&g_ui_samples);
    }
}
