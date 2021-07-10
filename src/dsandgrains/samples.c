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

UIInteractiveList g_ui_samples = {0};

void bind_samples_interactive_list(UIElements * line, ListItemOpt flag) {
    g_ui_samples.update_request = -1;
    if (line == NULL) {
        line = g_ui_samples.lines;
        g_ui_samples.window_offset = 0;
        dstudio_update_current_context(0, DSTUDIO_CLIENT_CONTEXTS_LEVEL);
    }
    DStudioContexts * samples = DSTUDIO_CURRENT_VOICE_CONTEXT->sub_contexts;
    g_ui_samples.source_data = (char*) samples->data;
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
    DStudioContexts * samples = DSTUDIO_CURRENT_VOICE_CONTEXT->sub_contexts;

    init_interactive_list(
        &g_ui_samples,
        ui_elements,
        lines_number,
        string_size,
        sizeof(SampleContext),
        &samples->count,
        (char*) samples->data,
        DSTUDIO_CLIENT_CONTEXTS_LEVEL,
        dstudio_select_context_from_list,
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
    DStudioContexts * samples = DSTUDIO_CURRENT_VOICE_CONTEXT->sub_contexts;
    SampleContext * new_sample_context = dstudio_realloc(
        samples->data,
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
    samples->data = new_sample_context;
    g_dstudio_active_contexts[DSTUDIO_CLIENT_CONTEXTS_LEVEL].current = (DStudioGenericContext *) &((SampleContext*)samples->data)[samples->index];
    DSTUDIO_CURRENT_SAMPLE_CONTEXT->identifier = dstudio_get_timestamp();
    DSTUDIO_CURRENT_SAMPLE_CONTEXT->parent = samples;
    
    DSTUDIO_CURRENT_SAMPLE_CONTEXT->start = dstudio_alloc(sizeof(ControllerValue), DSTUDIO_FAILURE_IS_FATAL);
    ControllerValue * start = DSTUDIO_CURRENT_SAMPLE_CONTEXT->start;
    start->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    start->computed = 0;
    start->multiplier = 1;
    start->offset = 0;
    start->context_identifier = DSTUDIO_CURRENT_SAMPLE_CONTEXT->identifier;
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, start);    
    g_ui_elements_struct.knob_sample_start.application_callback = request_sample_screen_range_update;

    DSTUDIO_CURRENT_SAMPLE_CONTEXT->end = dstudio_alloc(sizeof(ControllerValue), DSTUDIO_FAILURE_IS_FATAL);
    ControllerValue * end = DSTUDIO_CURRENT_SAMPLE_CONTEXT->end;
    end->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    end->computed = 1;
    end->multiplier = 1;
    end->offset = 0;
    end->context_identifier = DSTUDIO_CURRENT_SAMPLE_CONTEXT->identifier;;

    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, end); 
    g_ui_elements_struct.knob_sample_end.application_callback = request_sample_screen_range_update;

    DSTUDIO_CURRENT_SAMPLE_CONTEXT->amount = dstudio_alloc(sizeof(ControllerValue), DSTUDIO_FAILURE_IS_FATAL);
    ControllerValue * amount = DSTUDIO_CURRENT_SAMPLE_CONTEXT->amount;
    amount->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    amount->computed = 1;
    amount->multiplier = 2;
    amount->offset = 0;
    amount->context_identifier = DSTUDIO_CURRENT_SAMPLE_CONTEXT->identifier;

    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, amount);

    DSTUDIO_CURRENT_SAMPLE_CONTEXT->stretch = dstudio_alloc(sizeof(ControllerValue), DSTUDIO_FAILURE_IS_FATAL);
    ControllerValue * stretch = DSTUDIO_CURRENT_SAMPLE_CONTEXT->stretch;
    stretch->sensitivity = DSTUDIO_KNOB_SENSITIVITY_LINEAR;
    stretch->computed = 1;
    stretch->multiplier = 2;
    stretch->offset = 0;
    stretch->context_identifier = DSTUDIO_CURRENT_SAMPLE_CONTEXT->identifier;
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, stretch);

    strncpy(DSTUDIO_CURRENT_SAMPLE_CONTEXT->name, filename, g_ui_samples.string_size-1);
    #ifdef DSTUDIO_DEBUG
    printf(
        "%s %s %s\n",
        DSTUDIO_CURRENT_INSTANCE_CONTEXT->name,
        DSTUDIO_CURRENT_VOICE_CONTEXT->name,
        DSTUDIO_CURRENT_SAMPLE_CONTEXT->name
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
void select_sample_from_list() {
    DStudioContexts * samples = DSTUDIO_CURRENT_VOICE_CONTEXT->sub_contexts;   
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, DSTUDIO_CURRENT_SAMPLE_CONTEXT->amount);
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, DSTUDIO_CURRENT_SAMPLE_CONTEXT->stretch);
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, DSTUDIO_CURRENT_SAMPLE_CONTEXT->start);
    bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, DSTUDIO_CURRENT_SAMPLE_CONTEXT->end);
    bind_new_data_to_sample_screen(samples->count ? &DSTUDIO_CURRENT_SAMPLE_CONTEXT->shared_sample : 0);
}

// TODO : Smell like shit
UIElements * set_samples_ui_context_from_parent_voice_list() {
    UIElements * line;
    uint_fast32_t sample_index;
    DStudioContexts * samples = DSTUDIO_CURRENT_VOICE_CONTEXT->sub_contexts;
    sample_index = samples->index;

    if (sample_index < g_ui_samples.lines_number) {
        line = &g_ui_samples.lines[sample_index];
        g_ui_samples.window_offset = 0;
    }
    else {
        line = &g_ui_samples.lines[g_ui_voices.lines_number-1];
        g_ui_samples.window_offset = sample_index - g_ui_samples.lines_number + 1;
    }
    (void) line;
    if (samples->count) {
        g_dstudio_active_contexts[DSTUDIO_CLIENT_CONTEXTS_LEVEL].previous = 0;
        dstudio_update_current_context(sample_index, DSTUDIO_CLIENT_CONTEXTS_LEVEL);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_amount, DSTUDIO_CURRENT_SAMPLE_CONTEXT->amount);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_stretch, DSTUDIO_CURRENT_SAMPLE_CONTEXT->stretch);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_start, DSTUDIO_CURRENT_SAMPLE_CONTEXT->start);
        bind_and_update_ui_knob(&g_ui_elements_struct.knob_sample_end, DSTUDIO_CURRENT_SAMPLE_CONTEXT->end);
        bind_new_data_to_sample_screen(&DSTUDIO_CURRENT_SAMPLE_CONTEXT->shared_sample);
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

void update_samples_ui_list() {
    if (g_ui_samples.update_request) {
        update_interactive_list(&g_ui_samples);
    }
}
