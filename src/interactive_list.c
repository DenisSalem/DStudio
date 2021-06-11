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

#include "common.h"
#include "extensions.h"
#include "interactive_list.h"
#include "instances.h"
#include "sliders.h"
#include "text.h"
#include "text_pointer.h"

inline void bind_scroll_bar(UIInteractiveList * interactive_list, UIElements * scroll_bar) {
    scroll_bar->interactive_list = interactive_list;
    interactive_list->scroll_bar = scroll_bar;
    interactive_list->max_scroll_bar_offset= scroll_bar->instance_motions_buffer[0];
    scroll_bar->application_callback = scroll_by_slider;
    if (interactive_list->source_data == NULL) {
        scroll_bar->enabled = 0;
    }
}

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
) {
    interactive_list->lines_number = lines_number;
    interactive_list->string_size = string_size;
    interactive_list->highlight = ui_elements;
    interactive_list->lines = &ui_elements[1];
    for (uint_fast32_t i = 0; i < lines_number; i++) {
        interactive_list->lines[i].interactive_list = interactive_list;
    }
    interactive_list->stride = stride;
    interactive_list->source_data_count = source_data_count;
    interactive_list->source_data = source_data;
    interactive_list->select_callback = select_callback;
    interactive_list->edit_item_callback = edit_item_callback;
    interactive_list->editable = editable;
    interactive_list->highlight_step = highlight_step;
    interactive_list->highlight_offset_y = ui_elements->coordinates_settings.instance_offsets_buffer->y;
}

void scroll(UIInteractiveList * interactive_list, int_fast32_t direction) {
    uint_fast32_t do_action = 0;
    if (g_text_pointer_context.active) {
        return;
    }
    
    if (direction > 0) {
        do_action = interactive_list->window_offset + interactive_list->lines_number < *interactive_list->source_data_count;
    }
    else {
        do_action = interactive_list->window_offset > 0;
    }
    if (do_action) {
        interactive_list->window_offset += direction;
        interactive_list->index += -direction;
        interactive_list->update_index= -1;
        interactive_list->update_request= 1;

        if (interactive_list->index >= 0 && interactive_list->index < (int_fast32_t) interactive_list->lines_number) {
            select_item(
                &interactive_list->lines[interactive_list->index],
                DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
            );
        }
        else {
            interactive_list->update_highlight = 1;
            *interactive_list->highlight->instance_alphas_buffer = 0;
        }
    }
    if (interactive_list->scroll_bar) {
        update_scroll_bar(interactive_list);
    }
}

void scroll_by_slider(UIElements * ui_elements) {
    if (g_text_pointer_context.active) {
        return;
    }
    
    UIInteractiveList * interactive_list = ui_elements->interactive_list;
    
    // List empty? Nothing to do then.
    if (interactive_list->source_data == NULL) {
        return;
    }

    float slider_value = 1.0 - *(float *) ui_elements->application_callback_args;
    uint_fast32_t window_offset = (uint_fast32_t) round(slider_value * (*interactive_list->source_data_count - interactive_list->lines_number));
    
    // No actual motion? Just quit then.
    if (0 == interactive_list->window_offset - window_offset) {
        return;
    }

    interactive_list->index += interactive_list->window_offset - window_offset;
    
    interactive_list->window_offset = window_offset;
    interactive_list->update_index= -1;
    interactive_list->update_request = 1;
    if (interactive_list->index >= 0 && interactive_list->index < (int_fast32_t) interactive_list->lines_number) {
        select_item(
            &interactive_list->lines[interactive_list->index],
            DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
        );
    }
    else {
        interactive_list->update_highlight = 1;
        *interactive_list->highlight->instance_alphas_buffer = 0;
    }
}

void select_item(
    UIElements * self,
    ListItemOpt flag
) {
    uint_fast32_t lines_number = self->interactive_list->lines_number;
    UIInteractiveList * interactive_list = self->interactive_list;
    UIElements * highlight = interactive_list->highlight;

    for(uint_fast32_t i = 0; i < lines_number; i++) {
        if (&interactive_list->lines[i] == self) {
            if(flag & DSTUDIO_SELECT_ITEM_WITH_CALLBACK) {
                interactive_list->select_callback(i+interactive_list->window_offset);
            }
            if(
                (flag & (DSTUDIO_SELECT_ITEM_WITH_CALLBACK | DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK)) ||
                interactive_list->select_callback(i+interactive_list->window_offset)
            ) {
                interactive_list->lines[i].render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
                interactive_list->update_request = 1;
                interactive_list->update_highlight = 1;
                interactive_list->highlight->interactive_list = interactive_list;
                *interactive_list->highlight->instance_alphas_buffer = interactive_list->source_data ? 1 : 0;
                highlight->coordinates_settings.instance_offsets_buffer->y = interactive_list->highlight_offset_y + interactive_list->highlight_step * i;
                interactive_list->lines[interactive_list->previous_item_index].render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
                interactive_list->previous_item_index = i;
                interactive_list->index = i;
    
            }
            break;
        }
    }
}

void update_insteractive_list(
    UIInteractiveList * interactive_list
) {
    uint_fast32_t stride = interactive_list->stride;
    uint_fast32_t window_offset = interactive_list->window_offset;
    uint_fast32_t string_size = interactive_list->string_size;
    uint_fast32_t source_data_count = *interactive_list->source_data_count;
    int_fast32_t index = interactive_list->update_index;
    for (uint_fast32_t i = index < 0 ? 0 : index; i < interactive_list->lines_number; i++) {
        if (i < source_data_count) {
              update_text(
                &interactive_list->lines[i],
                (char *) &interactive_list->source_data[stride * (i + window_offset)],
                string_size
            );
        }
        else {
            update_text(
                &interactive_list->lines[i],
                "",
                string_size
            );
        }
    }

    if (interactive_list->scroll_bar) {
        interactive_list->scroll_bar->enabled = *interactive_list->source_data_count <= interactive_list->lines_number ? 0 : 1;
        if(!interactive_list->scroll_bar->render_state) {
            update_scroll_bar(interactive_list);
        }
    }
    if (interactive_list->update_highlight) {
        UIElements * highlight = interactive_list->highlight;
        highlight->coordinates_settings.previous_scissor.y = highlight->coordinates_settings.scissor.y;
        highlight->coordinates_settings.scissor.y = (1 + highlight->coordinates_settings.instance_offsets_buffer->y - highlight->coordinates_settings.scale_matrix[1].y) * (g_dstudio_viewport_height >> 1);
        highlight->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
        interactive_list->update_highlight = 0;
    }
    interactive_list->update_request = 0;
}

void update_scroll_bar(UIInteractiveList * interactive_list) {
    UIElements * scroll_bar = interactive_list->scroll_bar;
    uint_fast32_t multiplier;
    GLfloat relative_position = (GLfloat) interactive_list->window_offset / (GLfloat) (*interactive_list->source_data_count - interactive_list->lines_number);
    GLfloat height = (scroll_bar->areas.max_area_y - scroll_bar->areas.min_area_y) - scroll_bar->coordinates_settings.scale_matrix[1].y / (1.0 / (GLfloat) (g_dstudio_viewport_height));
    height = ((int_fast32_t) height % 2 == 0) ? height : height + 0.5; 
    relative_position *= height / (GLfloat) (g_dstudio_viewport_height >> 1);
    multiplier = relative_position / (1.0/(GLfloat) (g_dstudio_viewport_height >> 1));
    relative_position = multiplier * (1.0/(GLfloat) (g_dstudio_viewport_height >> 1));
    GLfloat motion = interactive_list->max_scroll_bar_offset - relative_position;
    update_ui_element_motion(scroll_bar, motion);
    compute_slider_in_motion_scissor_y(scroll_bar);
}
