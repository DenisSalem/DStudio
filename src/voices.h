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

#ifndef DSTUDIO_VOICES_H_INCLUDED
#define DSTUDIO_VOICES_H_INCLUDED

#include "audio_api.h"
#include "interactive_list.h"

typedef struct DStudioVoiceMidiBind_t {
    ControllerValue * controller_value;
    UIElements * ui_element;
} DStudioVoiceMidiBind;

typedef struct VoiceContext_t {
    char name[DSTUDIO_INSTANCE_NAME_LENGTH];
    void * sub_contexts;
    void * midi_data_buffer;
    AudioPort ports;
    DStudioVoiceMidiBind midi_binds[256];
} VoiceContext;

typedef struct Voices_t {
    VoiceContext * contexts;
    uint_fast32_t count;
    uint_fast32_t index;
} Voices;

extern VoiceContext * g_current_active_voice;
extern UIInteractiveList g_ui_voices;
void bind_voices_interactive_list(UIElements * line);
extern void (*bind_sub_context_interactive_list)(UIElements * line, ListItemOpt flag);
extern UIElements * (*setup_sub_context_interactive_list)();

void init_voices_interactive_list(
    UIElements * ui_elements,
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
    GLfloat item_offset_y
);

UIElements * new_voice();

uint_fast32_t select_voice_from_list(
    uint_fast32_t index
);

void setup_voice_sub_context(
    uint_fast32_t size,
    void (*sub_context_interactive_list_binder)(UIElements * lines, ListItemOpt flag),
    UIElements * (*sub_context_interactive_list_setter)()
);

void update_current_voice(uint_fast32_t index);
void update_voices_ui_list();
#endif
