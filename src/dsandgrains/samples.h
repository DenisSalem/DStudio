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

#ifndef DSANDGRAINS_SAMPLES_H_INCLUDED
#define DSANDGRAINS_SAMPLES_H_INCLUDED

#include "../common.h"
#include "../interactive_list.h"
#include "../knob.h"
#include "../samples.h"
#include "../voices.h"

typedef struct SampleContext_t {
    char name[DSTUDIO_INSTANCE_NAME_LENGTH];
    DStudioContexts * parent;
    SharedSample shared_sample; // TODO MUST BE A POINTER !!!
    int_fast64_t processed_index;
    ControllerValue * start;
    ControllerValue * end;
    ControllerValue * amount;
    ControllerValue * stretch;
    double  identifier;
} SampleContext;

typedef struct Samples_t{
    SampleContext * contexts;
    uint_fast32_t count;
    uint_fast32_t index;
} Samples;

extern SampleContext * g_current_active_sample; 
extern UIInteractiveList g_ui_samples;

void bind_samples_interactive_list(UIElements * line, ListItemOpt flag);

void init_samples_interactive_list(
    UIElements * ui_elements,
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
    GLfloat item_offset_y
);

UIElements * new_sample(char * filename, SharedSample shared_sample);

uint_fast32_t select_sample_from_list(
    uint_fast32_t index
);

UIElements * set_samples_ui_context_from_parent_voice_list();

void update_current_sample(
    uint_fast32_t index
);

void update_samples_ui_list();

#endif
