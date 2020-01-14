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

#ifndef DSTUDIO_INTERACTIVE_LIST_H_INCLUDED
#define DSTUDIO_INTERACTIVE_LIST_H_INCLUDED

#include <semaphore.h>

#include "ui.h"

typedef struct UIElements_t UIElements;

typedef struct UIInteractiveList_t {
    UIElements * lines;
    unsigned int lines_number;
    unsigned int string_size;
    unsigned int window_offset;
    unsigned int stride;
    unsigned int * source_data_count;
    char ** source_data;
    ThreadControl * thread_bound_control;
    int update_request;
} UIInteractiveList;

//~ typedef struct UIInteractiveListSetting_t {
    //~ GLfloat gl_x;
    //~ GLfloat gl_y;
    //~ GLfloat min_area_x;
    //~ GLfloat max_area_x;
    //~ GLfloat min_area_y;
    //~ GLfloat max_area_y;
    //~ GLfloat offset;
    //~ GLfloat area_offset;
//~ } UIInteractiveListSetting;

//~ typedef struct InteractiveListContext_t {
        //~ void (*application_callback)(unsigned int index);
        //~ char * (*get_item_name_callback) (unsigned int index);
        //~ void (*sub_ui_element_update_callback)();
        //~ UIInteractiveList * related_list;
        //~ int render_flag;
//~ } InteractiveListContext;

//~ void configure_ui_interactive_list(
    //~ UIElements * ui_elements,
    //~ void * params
//~ );

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * lines,
    unsigned int lines_number,
    unsigned int string_size,
    unsigned int stride,
    unsigned int * source_data_count,
    char ** source_data,
    ThreadControl * thread_bound_control
);

//~ void select_element_from_list(
    //~ void * args
//~ );

/*
 * negative item_index means update all the item.
 */
void update_insteractive_list(
    UIInteractiveList * interactive_list
);

void scroll_down(UIElements * self);
void scroll_up(UIElements * self);

//~ void update_insteractive_list_shadow(
    //~ int context_type,
    //~ UIInteractiveList * interactive_list
//~ );

//~ extern unsigned char g_group_identifier;
#endif
