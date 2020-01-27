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

#ifndef DSTUDIO_VOICES_H_INCLUDED
#define DSTUDIO_VOICES_H_INCLUDED

#include "interactive_list.h"

typedef struct VoiceContext_t {
    char name[33];
} VoiceContext;

typedef struct Voices_t {
    VoiceContext * contexts;
    unsigned int count;
    unsigned int index;
    ThreadControl thread_control;
} Voices;

extern VoiceContext * g_current_active_voice;
extern UIInteractiveList g_ui_voices;
void bind_voices_interactive_list();

void init_voices_interactive_list(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
);

int new_voice(unsigned int use_mutex);

unsigned int select_voice_from_list(
    unsigned int index
);

void update_current_voice(unsigned int index);
void update_voices_ui_list();
#endif
