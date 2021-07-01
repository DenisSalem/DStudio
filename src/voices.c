/*
 * Copyright 2019, 2021  Denis Salem
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

#include "common.h"
#include "info_bar.h"
#include "instances.h"
#include "text_pointer.h"
#include "voices.h" 

// TODO : TO REMOVE
//VoiceContext * g_current_active_voice = 0; 
UIInteractiveList g_ui_voices = {0};
UIElements * g_ui_elements = {0};

static VoiceContext * s_previous_active_voice = 0; 

void (*bind_sub_context_interactive_list)(UIElements * line, ListItemOpt flag) = 0;
UIElements * (*setup_sub_context_interactive_list)() = 0;

static UIElements * s_ui_elements;
static uint_fast32_t s_lines_number;
static uint_fast32_t s_string_size;
static GLfloat s_item_offset_y;

void bind_voices_interactive_list(UIElements * line) {
    DStudioContexts * voices = NULL;
    g_ui_voices.update_index = -1;
    if (line == NULL) {
        line = g_ui_voices.lines;
        g_ui_voices.window_offset = 0;
        update_current_voice(0);
    }
    voices = DSTUDIO_CURRENT_INSTANCE_CONTEXT->voices;
    g_dstudio_active_contexts[DSTUDIO_VOICE_CONTEXT_LEVEL].current = &((VoiceContext*) voices->data)[voices->index];
    g_ui_voices.source_data = (char*) &((VoiceContext*) voices->data)->name;
    g_ui_voices.source_data_count = &voices->count;
    select_item(
        line,
        DSTUDIO_SELECT_ITEM_WITH_CALLBACK
    );
}

void init_voices_interactive_list(
    UIElements * ui_elements,
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
    GLfloat item_offset_y
) {
    s_ui_elements = ui_elements;
    s_lines_number = lines_number;
    s_string_size = string_size;
    s_item_offset_y = item_offset_y;
    DStudioContexts * voices = DSTUDIO_CURRENT_INSTANCE_CONTEXT->voices;

    init_interactive_list(
        &g_ui_voices,
        s_ui_elements,
        s_lines_number,
        s_string_size,
        sizeof(VoiceContext),
        &voices->count,
        (char *) voices->data,
        select_voice_from_list,
        _rename_active_context_audio_port,
        1,
        s_item_offset_y
    );
    #ifdef DSTUDIO_DEBUG
        strcat(&g_ui_voices.trace[0], "g_ui_voices");
    #endif
}

UIElements * new_voice() {
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_NO_DATA_PROCESSING);
    UIElements * line = 0;
    DStudioContexts * voices = DSTUDIO_CURRENT_INSTANCE_CONTEXT->voices;

    VoiceContext * new_voice_context = dstudio_realloc(
        voices->data,
        (voices->count + 1) * sizeof(VoiceContext)
    );
    if (new_voice_context == NULL) {
        dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
        return 0;
    }
    explicit_bzero(
        &new_voice_context[voices->count],
        sizeof(VoiceContext)
    );

    voices->index = voices->count++;
    voices->data = new_voice_context;
    g_dstudio_active_contexts[DSTUDIO_VOICE_CONTEXT_LEVEL].current = &((VoiceContext*)voices->data)[voices->index];
    // TODO: at every reallocation, every contexts parent pointer must be refreshed.
    // OR DStudioContexts become a manually allocated memory object.
    DSTUDIO_CURRENT_VOICE_CONTEXT->parent = voices;
    DSTUDIO_CURRENT_VOICE_CONTEXT->sub_contexts = dstudio_alloc(
        g_dstudio_client_context_size,
        DSTUDIO_FAILURE_IS_FATAL
    );
    
    sprintf(DSTUDIO_CURRENT_VOICE_CONTEXT->name, "Voice %" PRIuFAST32, voices->count);
    #ifdef DSTUDIO_DEBUG
    printf("%s %s\n", DSTUDIO_CURRENT_INSTANCE_CONTEXT->name, DSTUDIO_CURRENT_VOICE_CONTEXT->name);
    #endif

    if (voices->count > g_ui_voices.lines_number) {
        g_ui_voices.update_index = -1;
        g_ui_voices.window_offset = voices->count - g_ui_voices.lines_number;
    }
    else {
        g_ui_voices.update_index = voices->index;
        g_ui_voices.window_offset = 0;
    }

    if (s_ui_elements) {
        line = &g_ui_voices.lines[voices->index-g_ui_voices.window_offset];
        bind_voices_interactive_list(line);
        bind_sub_context_interactive_list(
            setup_sub_context_interactive_list(),
            DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
        );
    }
    
    // Audio API port creation
    // TODO : It doesn't need argument anymore
    dstudio_audio_api_high_level_port_registration(DSTUDIO_CURRENT_INSTANCE_CONTEXT, DSTUDIO_CURRENT_VOICE_CONTEXT);
    
    g_ui_voices.update_request = 1;
    
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
    
    return line;
}

uint_fast32_t select_voice_from_list(
    uint_fast32_t index
) {
    DStudioContexts * voices = DSTUDIO_CURRENT_INSTANCE_CONTEXT->voices;
    if ((index != voices->index || DSTUDIO_CURRENT_VOICE_CONTEXT != s_previous_active_voice) && index < voices->count) {
        update_current_voice(index);
        bind_sub_context_interactive_list(
            setup_sub_context_interactive_list(),
            DSTUDIO_SELECT_ITEM_WITH_CALLBACK
        );
        return 1;
    }
    return 0;
}

// TODO : Callback only handles interactive list. It lake of other contextual UI management.
void setup_voice_sub_context(
    uint_fast32_t size,
    void (*sub_context_interactive_list_binder)(UIElements * lines, ListItemOpt flag),
    UIElements * (*sub_context_interactive_list_setter)()
) {
    g_dstudio_client_context_size = size;
    bind_sub_context_interactive_list = sub_context_interactive_list_binder;
    setup_sub_context_interactive_list = sub_context_interactive_list_setter;
}

void update_current_voice(uint_fast32_t index) {
    DStudioContexts * voices = DSTUDIO_CURRENT_INSTANCE_CONTEXT->voices;
    voices->index = index;
    g_dstudio_active_contexts[DSTUDIO_VOICE_CONTEXT_LEVEL].previous = g_dstudio_active_contexts[DSTUDIO_VOICE_CONTEXT_LEVEL].current;
    g_dstudio_active_contexts[DSTUDIO_VOICE_CONTEXT_LEVEL].current = &((VoiceContext*)voices->data)[index];
    g_midi_capture_state = DSTUDIO_AUDIO_API_MIDI_CAPTURE_NONE;
    
    dstudio_update_info_text("");
    setup_sub_context_interactive_list();
}

void update_voices_ui_list() {
    if (g_ui_voices.update_request) {
        update_interactive_list(&g_ui_voices);
        g_ui_voices.update_request = 0;
    }
}
