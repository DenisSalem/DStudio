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

#ifndef DSTUDIO_INTERACTIVE_LIST_H_INCLUDED
#define DSTUDIO_INTERACTIVE_LIST_H_INCLUDED

#include <semaphore.h>

#include "ui.h"

typedef enum ListItemOpt_t {
    DSTUDIO_SELECT_ITEM_WITH_CALLBACK_TEST = 0,
    DSTUDIO_SELECT_ITEM_WITH_CALLBACK = 1,
    DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK = 2,
} ListItemOpt;

typedef struct UIElements_t UIElements;

typedef struct UIInteractiveList_t {
    UIElements * highlight;
    UIElements * scroll_bar;
    UIElements * lines;
    uint_fast32_t lines_number;
    uint_fast32_t string_size;
    uint_fast32_t window_offset;
    uint_fast32_t stride;
    uint_fast32_t * source_data_count;
    int_fast32_t update_index;
    uint_fast32_t (*select_callback)(uint_fast32_t index);
    // Will be called when writting the item name.
    uint_fast32_t (*edit_item_callback)(uint_fast32_t index);
    uint_fast8_t  editable;
    uint_fast8_t  update_highlight;
    uint_fast8_t  update_request;
    char * source_data;
    
    GLfloat highlight_offset_y;
    GLfloat highlight_step;
    GLfloat max_scroll_bar_offset;
    uint_fast32_t previous_item_index;
    int_fast32_t index;
    #ifdef DSTUDIO_DEBUG
        char trace[32];
    #endif
} UIInteractiveList;

void bind_scroll_bar(
    UIInteractiveList * interactive_list,
    UIElements * scroll_bar
);

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * ui_elements,
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
    uint_fast32_t stride,
    uint_fast32_t * source_data_count,
    char * source_data,
    uint_fast32_t (*select_callback)(uint_fast32_t index),
    uint_fast32_t (*edit_item_callback)(uint_fast32_t index),
    uint_fast32_t editable,
    GLfloat highlight_step
);

void update_interactive_list(
    UIInteractiveList * interactive_list
);

void scroll(
    UIInteractiveList * interactive_list,
    int_fast32_t direction
);

void scroll_by_slider(
    UIElements * ui_elements
);

void select_item(
    UIElements * self,
    ListItemOpt do_not_use_callback
);

void update_scroll_bar(UIInteractiveList * interactive_list);

#endif
