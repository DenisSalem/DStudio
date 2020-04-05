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

#ifndef DSTUDIO_SAMPLES_H_INCLUDED
#define DSTUDIO_SAMPLES_H_INCLUDED

#include "../common.h"
#include "../interactive_list.h"
#include "../voices.h"

typedef struct SampleContext_t {
    char name[DSTUDIO_INSTANCE_NAME_LENGTH];
} SampleContext;

typedef struct Samples_t{
    SampleContext * contexts;
    unsigned int count;
    unsigned int index;
} Samples;

extern SampleContext * g_current_active_sample; 
extern ThreadControl g_samples_thread_control;
extern UIInteractiveList g_ui_samples;

void init_samples_interactive_list(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
);

unsigned int select_sample_from_list(
    unsigned int index
);

void update_current_sample(
    unsigned int index
);

#endif
