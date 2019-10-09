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

#include "instances.h"
#include "voices.h"

VoiceContext * g_current_active_voice = 0; 
UIInteractiveList g_ui_voices = {0};

void init_voices_ui(UIElements * lines, unsigned int lines_number, unsigned int string_size) {
    init_interactive_list(&g_ui_voices, lines, lines_number, string_size);
}


int new_voice() {
    VoiceContext * new_voice_context = realloc(
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
    printf("%s %s\n", g_current_active_instance->name, g_current_active_voice->name);
    return 1;
}

void update_voices_text() {
    update_insteractive_list(
        DSTUDIO_CONTEXT_INSTANCES,
        g_ui_voices.window_offset,
        g_ui_voices.lines_number,
        g_current_active_instance->voices.count,
        g_ui_voices.lines,
        g_ui_voices.string_size, 
        &g_current_active_instance->voices.contexts[0]
    );
}
