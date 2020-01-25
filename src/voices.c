/*
 * Copyright 2019 Denis Salem
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
#include "voices.h"

VoiceContext * g_current_active_voice = 0; 
UIInteractiveList g_ui_voices = {0};
UIElements * g_ui_elements = {0};

static UIElements * s_ui_elements;
static unsigned int s_lines_number;
static unsigned int s_string_size;
static GLfloat s_item_offset_y;

void bind_voices_interactive_list() {
    init_interactive_list(
        &g_ui_voices,
        s_ui_elements,
        s_lines_number,
        s_string_size,
        sizeof(InstanceContext),
        &g_current_active_instance->voices.count,
        (char **) &g_current_active_instance->voices.contexts,
        &g_current_active_instance->voices.thread_control,
        select_instance_from_list,
        1,
        s_item_offset_y
    );
    g_current_active_instance->voices.thread_control.ready = 1;
    g_current_active_instance->voices.thread_control.update = 1;
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
    bind_voices_interactive_list();
}

int new_voice() {
    VoiceContext * new_voice_context = dstudio_realloc(
        g_current_active_instance->voices.contexts,
        (g_current_active_instance->voices.count + 1) * sizeof(VoiceContext)
    );
    if (new_voice_context == NULL) {
        return 0;
    }
    g_current_active_instance->voices.contexts = new_voice_context;
    g_current_active_voice = &g_current_active_instance->voices.contexts[g_current_active_instance->voices.count];
    explicit_bzero(g_current_active_voice, sizeof(VoiceContext));
    g_current_active_instance->voices.count++;
    sprintf(g_current_active_voice->name, "Voice %d", g_current_active_instance->voices.count);
    #ifdef DSTUDIO_DEBUG
    printf("%s %s\n", g_current_active_instance->name, g_current_active_voice->name);
    #endif
    sem_init(&g_current_active_instance->voices.thread_control.mutex, 0, 1);

    if (s_ui_elements) {
        bind_voices_interactive_list();
    }
    return 1;
}

//~ void scroll_voices(void * args) {
    //~ (void) args;
//~ }

void update_voices_ui_list() {
    update_insteractive_list(&g_ui_voices);
}
