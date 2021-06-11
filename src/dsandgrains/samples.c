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
#include "../info_bar.h"
#include "../instances.h"
#include "../knob.h"
#include "samples.h"
#include "sample_screen.h"
#include "ui.h"


SampleContext * g_current_active_sample = 0;
UIInteractiveList g_ui_samples = {0};

static SampleContext * s_previous_active_sample = 0;

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
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
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
        &samples->contexts->name[0],
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
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_NO_DATA_PROCESSING);

    UIElements * line = 0;
    Samples * samples = g_current_active_voice->sub_contexts;
    SampleContext * new_sample_context = dstudio_realloc(
        samples->contexts,
        (samples->count + 1) * sizeof(SampleContext)
    );
    if (new_sample_context == NULL) {
        // TODO: LOG FAILURE
        dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
        dstudio_update_info_text("Failed to import sample.");
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
    
    KnobValue * start = &g_current_active_sample->start;
    start->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    start->computed = 0;
    start->multiplier = 1;
    start->offset = 0;
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, start);    
    g_ui_elements_struct.knob_sample_start.application_callback = request_sample_screen_range_update;
    
    KnobValue * end = &g_current_active_sample->end;
    end->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    end->computed = 1;
    end->multiplier = 1;
    end->offset = 0;
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, end); 
    g_ui_elements_struct.knob_sample_end.application_callback = request_sample_screen_range_update;

    KnobValue * amount = &g_current_active_sample->amount;
    amount->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    amount->computed = 1;
    amount->multiplier = 2;
    amount->offset = 0;
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, amount);

    KnobValue * stretch = &g_current_active_sample->stretch;
    stretch->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    stretch->computed = 1;
    stretch->multiplier = 2;
    stretch->offset = 0;
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, stretch);

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
    
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
    
    return line;
}

// TODO: the following could be generalized and not implemented by consumer.
uint_fast32_t select_sample_from_list(
    uint_fast32_t index
) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;   

    if ((index != samples->index || g_current_active_sample != s_previous_active_sample) && index < samples->count) {
        update_current_sample(index);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, &g_current_active_sample->amount);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, &g_current_active_sample->stretch);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, &g_current_active_sample->start);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, &g_current_active_sample->end);
        bind_new_data_to_sample_screen(samples->count ? &g_current_active_sample->shared_sample : 0);
        return 1;
    }
    
    return 0;
}

// TODO : Smell like shit
UIElements * set_samples_ui_context_from_parent_voice_list() {
    UIElements * line;
    uint_fast32_t sample_index;
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
    if (samples->count) {
        s_previous_active_sample = 0;
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, &samples->contexts[samples->index].amount);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, &samples->contexts[samples->index].stretch);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, &samples->contexts[samples->index].start);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, &samples->contexts[samples->index].end);
        bind_new_data_to_sample_screen(&samples->contexts[samples->index].shared_sample);
    }
    else {
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, 0);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, 0);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, 0);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, 0);
        bind_new_data_to_sample_screen(NULL);
    }
    return line;
}

// TODO: the following could be generalized and not implemented by consumer.
void update_current_sample(uint_fast32_t index) {
    Samples * samples = (Samples * ) g_current_active_voice->sub_contexts;
    samples->index = index;
    s_previous_active_sample = g_current_active_sample;
    g_current_active_sample = &samples->contexts[index];
}

void update_samples_ui_list() {
    if (g_ui_samples.update_request) {
        update_insteractive_list(&g_ui_samples);
    }
}
