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

VoiceContext * g_current_active_voice = 0; 
UIInteractiveList g_ui_voices = {0};
UIElements * g_ui_elements = {0};

static VoiceContext * s_previous_active_voice = 0; 

void (*bind_sub_context_interactive_list)(UIElements * line, ListItemOpt flag) = 0;
UIElements * (*setup_sub_context_interactive_list)() = 0;

char s_audio_port_name_left_buffer[DSTUDIO_PORT_NAME_LENGHT] = {0};
char s_audio_port_name_right_buffer[DSTUDIO_PORT_NAME_LENGHT] = {0};
char s_midi_port_name_buffer[DSTUDIO_PORT_NAME_LENGHT] = {0};

static UIElements * s_ui_elements;
static unsigned int s_lines_number;
static unsigned int s_string_size;
static GLfloat s_item_offset_y;
static unsigned int s_sub_context_size = 0;

void bind_voices_interactive_list(UIElements * line) {
    g_ui_voices.update_index = -1;
    if (line == NULL) {
        line = g_ui_voices.lines;
        g_ui_voices.window_offset = 0;
        update_current_voice(0);
    }
    g_current_active_voice = &g_current_active_instance->voices.contexts[g_current_active_instance->voices.index];
    g_ui_voices.source_data = (char*) &g_current_active_instance->voices.contexts->name;
    g_ui_voices.source_data_count = &g_current_active_instance->voices.count;
    select_item(
        line,
        DSTUDIO_SELECT_ITEM_WITH_CALLBACK
    );
}

void init_voices_interactive_list(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
) {
    s_ui_elements = ui_elements;
    s_lines_number = lines_number;
    s_string_size = string_size;
    s_item_offset_y = item_offset_y;
    
    init_interactive_list(
        &g_ui_voices,
        s_ui_elements,
        s_lines_number,
        s_string_size,
        sizeof(VoiceContext),
        &g_current_active_instance->voices.count,
        g_current_active_instance->voices.contexts->name,
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

    VoiceContext * new_voice_context = dstudio_realloc(
        g_current_active_instance->voices.contexts,
        (g_current_active_instance->voices.count + 1) * sizeof(VoiceContext)
    );
    if (new_voice_context == NULL) {
        dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
        return 0;
    }
    explicit_bzero(
        &new_voice_context[g_current_active_instance->voices.count],
        sizeof(VoiceContext)
    );

    g_current_active_instance->voices.index = g_current_active_instance->voices.count++;

    g_current_active_instance->voices.contexts = new_voice_context;
    g_current_active_voice = &g_current_active_instance->voices.contexts[g_current_active_instance->voices.index];
    g_current_active_voice->sub_contexts = dstudio_alloc(
        s_sub_context_size,
        DSTUDIO_FAILURE_IS_FATAL
    );
    
    sprintf(g_current_active_voice->name, "Voice %d", g_current_active_instance->voices.count);
    #ifdef DSTUDIO_DEBUG
    printf("%s %s\n", g_current_active_instance->name, g_current_active_voice->name);
    #endif

    if (g_current_active_instance->voices.count > g_ui_voices.lines_number) {
        g_ui_voices.window_offset = g_current_active_instance->voices.count - g_ui_voices.lines_number;
        g_ui_voices.update_index = -1;
    }
    else {
        g_ui_voices.update_index = g_current_active_instance->voices.index;
        g_ui_voices.window_offset = 0;
    }

    line = &g_ui_voices.lines[g_current_active_instance->voices.index-g_ui_voices.window_offset];
    if (s_ui_elements) {
        bind_voices_interactive_list(line);
        bind_sub_context_interactive_list(
            setup_sub_context_interactive_list(),
            DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
        );

    }
    
    strcpy((char *) &s_audio_port_name_left_buffer, g_current_active_instance->name);
    strcpy((char *) &s_audio_port_name_right_buffer, g_current_active_instance->name);
    strcpy((char *) &s_midi_port_name_buffer, g_current_active_instance->name);
    strcat((char *) &s_audio_port_name_left_buffer, " > ");
    strcat((char *) &s_audio_port_name_right_buffer, " > ");
    strcat((char *) &s_midi_port_name_buffer, " > ");
    strcat((char *) &s_audio_port_name_left_buffer, g_current_active_voice->name);
    strcat((char *) &s_audio_port_name_right_buffer, g_current_active_voice->name);
    strcat((char *) &s_midi_port_name_buffer, g_current_active_voice->name);
    strcat((char *) &s_audio_port_name_left_buffer, " > L");
    strcat((char *) &s_audio_port_name_right_buffer, " >  R");
            
    // TODO HANDLE FAILURE
    register_stereo_output_port(
        &g_current_active_voice->ports,
        (const char *) &s_audio_port_name_left_buffer,
        (const char *) &s_audio_port_name_right_buffer
    );
    
    register_midi_port(
        &g_current_active_voice->ports,
        (const char *) &s_midi_port_name_buffer
    );
    
    g_ui_voices.update_request = 1;
    
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
    
    return line;
}

unsigned int select_voice_from_list(
    unsigned int index
) {
    if ((index != g_current_active_instance->voices.index || g_current_active_voice != s_previous_active_voice) && index < g_current_active_instance->voices.count) {
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
    unsigned int size,
    void (*sub_context_interactive_list_binder)(UIElements * lines, ListItemOpt flag),
    UIElements * (*sub_context_interactive_list_setter)()
) {
    s_sub_context_size = size;
    bind_sub_context_interactive_list = sub_context_interactive_list_binder;
    setup_sub_context_interactive_list = sub_context_interactive_list_setter;
}

void update_current_voice(unsigned int index) {
    g_current_active_instance->voices.index = index;
    s_previous_active_voice = g_current_active_voice;
    g_current_active_voice = &g_current_active_instance->voices.contexts[index];
    g_midi_capture_state = DSTUDIO_AUDIO_API_MIDI_CAPTURE_NONE;
    update_info_text("");
    setup_sub_context_interactive_list();
}

void update_voices_ui_list() {
    if (g_ui_voices.update_request) {
        update_insteractive_list(&g_ui_voices);
        g_ui_voices.update_request = 0;
    }
}
