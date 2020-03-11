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

#ifndef DSTUDIO_INTERACTIVE_LIST_H_INCLUDED
#define DSTUDIO_INTERACTIVE_LIST_H_INCLUDED

#include <semaphore.h>

#include "ui.h"

typedef enum SelectItemOpt_t {
    DSTUDIO_SELECT_ITEM_WITH_CALLBACK = 0,
    DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK = 1,
} SelectItemOpt;

typedef struct UIElements_t UIElements;

typedef struct UIInteractiveList_t {
    UIElements * highlight;
    UIElements * scroll_bar;
    UIElements * lines;
    unsigned int lines_number;
    unsigned int string_size;
    unsigned int window_offset;
    unsigned int stride;
    unsigned int * source_data_count;
    char * source_data;
    ThreadControl * thread_bound_control;
    int update_request;
    unsigned int (*select_callback)(unsigned int index);
    unsigned char editable;
    unsigned char update_highlight;
    GLfloat highlight_offset_y;
    GLfloat highlight_step;
    GLfloat max_scroll_bar_offset;
    unsigned int previous_item_index;
    int index;
} UIInteractiveList;

void bind_scroll_bar(
    UIInteractiveList * interactive_list,
    UIElements * scroll_bar
);

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    unsigned int stride,
    unsigned int * source_data_count,
    char * source_data,
    ThreadControl * thread_bound_control,
    unsigned int (*select_callback)(unsigned int index),
    unsigned int editable,
    GLfloat highlight_step
);

void update_insteractive_list(
    UIInteractiveList * interactive_list
);

void scroll(
    UIInteractiveList * interactive_list,
    int direction
);

void scroll_by_slider(
    UIElements * ui_elements
);

void select_item(
    UIElements * self,
    unsigned int do_not_use_callback
);

void update_scroll_bar(UIInteractiveList * interactive_list);

#endif
