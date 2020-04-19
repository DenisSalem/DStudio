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

#ifndef DSTUDIO_INSTANCES_H_INCLUDED
#define DSTUDIO_INSTANCES_H_INCLUDED

#include "interactive_list.h"
#include "voices.h"

typedef struct InstanceContext_t {
    char name[DSTUDIO_INSTANCE_NAME_LENGTH];
    int identifier;
    Voices voices;
} InstanceContext;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
    unsigned int index;
    ThreadControl thread_control;
} Instances;

extern Instances g_instances;
extern InstanceContext * g_current_active_instance;
extern UIInteractiveList g_ui_instances;

FILE * add_instance_file_descriptor();

void exit_instances_management_thread();

void init_instances_management_thread(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
);
void * instances_management_thread(void * args);

void new_instance(
    const char * given_directory,
    const char * process_name
);

unsigned int select_instance_from_list(unsigned int index);

void update_instances_ui_list();

void update_current_instance(unsigned int index);
#endif
