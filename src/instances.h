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

#ifndef DSTUDIO_INSTANCES_H_INCLUDED
#define DSTUDIO_INSTANCES_H_INCLUDED

#include "interactive_list.h"
#include "voices.h"

typedef struct InstanceContext_t {
    char name[DSTUDIO_INSTANCE_NAME_LENGTH];
    int_fast32_t identifier;
    DStudioContexts voices;
} InstanceContext;

// TODO DEPRECATED
//~ typedef struct Instances_t {
    //~ InstanceContext * contexts;
    //~ uint_fast32_t count;
    //~ uint_fast32_t index;
//~ } Instances;

extern DStudioContexts g_instances;
extern InstanceContext * g_current_active_instance;
extern UIInteractiveList g_ui_instances;

FILE * add_instance_file_descriptor();

uint_fast32_t _rename_active_context_audio_port(uint_fast32_t index);

void init_instances_interactive_list(
    UIElements * ui_elements,
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
    GLfloat item_offset_y
);

void init_instance_management_backend();

void instances_management();

void new_instance(
    const char * given_directory,
    const char * process_name
);

uint_fast32_t select_instance_from_list(uint_fast32_t index);

void update_ui_instances_list();

void update_current_instance(uint_fast32_t index);
#endif
