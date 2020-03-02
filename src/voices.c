/*
 * Copyright 2019, 2020  Denis Salem
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
#include "instances.h"
#include "text_pointer.h"
#include "voices.h"

VoiceContext * g_current_active_voice = 0; 
UIInteractiveList g_ui_voices = {0};
UIElements * g_ui_elements = {0};
ThreadControl g_voices_thread_control = {0};

static UIElements * s_ui_elements;
static unsigned int s_lines_number;
static unsigned int s_string_size;
static GLfloat s_item_offset_y;

void bind_voices_interactive_list(UIElements * line) {
    g_ui_voices.update_request = -1;
    if (line == NULL) {
        line = g_ui_voices.lines;
        g_ui_voices.window_offset = 0;
        update_current_voice(0);
    }
    g_ui_voices.source_data = (char*) &g_current_active_instance->voices.contexts->name;
    g_ui_voices.source_data_count = &g_current_active_instance->voices.count;
    select_item(
        line,
        DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
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
        &g_voices_thread_control,
        select_voice_from_list,
        1,
        s_item_offset_y
    );
}

UIElements * new_voice(unsigned int use_mutex) {
    UIElements * line = 0;
    if(use_mutex) {
        sem_wait(g_voices_thread_control.shared_mutex);
    }
    VoiceContext * new_voice_context = dstudio_realloc(
        g_current_active_instance->voices.contexts,
        (g_current_active_instance->voices.count + 1) * sizeof(VoiceContext)
    );
    if (new_voice_context == NULL) {
        if(use_mutex) {
            sem_post(g_voices_thread_control.shared_mutex);
        }
        return 0;
    }
    explicit_bzero(
        &new_voice_context[g_current_active_instance->voices.count],
        sizeof(VoiceContext)
    );

    g_current_active_instance->voices.index = g_current_active_instance->voices.count++;

    g_current_active_instance->voices.contexts = new_voice_context;
    g_current_active_voice = &g_current_active_instance->voices.contexts[g_current_active_instance->voices.index];

    sprintf(g_current_active_voice->name, "Voice %d", g_current_active_instance->voices.count);
    #ifdef DSTUDIO_DEBUG
    printf("%s %s\n", g_current_active_instance->name, g_current_active_voice->name);
    #endif

    if (g_current_active_instance->voices.count > g_ui_voices.lines_number) {
        g_ui_voices.window_offset = g_current_active_instance->voices.count - g_ui_voices.lines_number;
        g_ui_voices.update_request = -1;
    }
    else {
        g_ui_voices.update_request = g_instances.index;
        g_ui_voices.window_offset = 0;
    }

    line = &g_ui_voices.lines[g_current_active_instance->voices.index-g_ui_voices.window_offset];
    if (s_ui_elements) {
        bind_voices_interactive_list(line);
    }
    if(use_mutex) {
        sem_post(g_voices_thread_control.shared_mutex);
    }
    return line;
}

unsigned int select_voice_from_list(
    unsigned int index
) {
    if (index != g_current_active_instance->voices.index && index < g_current_active_instance->voices.count) {
        update_current_voice(index);
        return 1;
    }
    return 0;
}

void update_current_voice(unsigned int index) {
    g_current_active_instance->voices.index = index;
    g_current_active_voice = &g_current_active_instance->voices.contexts[index];
}

void update_voices_ui_list() {
    update_insteractive_list(&g_ui_voices);
}
