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

#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "buttons.h"
#include "extensions.h"
#include "window_management.h"

ButtonsManagement g_buttons_management = {0};

void * buttons_management_thread(void * args) {
    (void) args;
    double timestamp = 0;
    double elapsed_time = 0;
    int pointer_x, pointer_y, texture_index;
    while(!g_buttons_management.thread_control.ready) {
        usleep(1000);
    }
    
    while(1) {
        usleep(g_framerate * 10);
        sem_wait(&g_buttons_management.thread_control.mutex);

        if (g_buttons_management.thread_control.cut_thread) {
            sem_post(&g_buttons_management.thread_control.mutex);
            break;
        }

        for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) {
            if (g_ui_elements_array[i].type == DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE) {
                timestamp = g_ui_elements_array[i].timestamp;
                if (timestamp == 0) {
                    continue;
                }
                elapsed_time = get_timestamp() - timestamp;
    
                if (elapsed_time > 0.05) {
                    if (g_ui_elements_array[i].texture_index == 0 && g_dstudio_mouse_state == 0) {
                        g_ui_elements_array[i].timestamp = 0;
                        continue;
                    }
                    if (g_ui_elements_array[i].texture_index == 1 && g_dstudio_mouse_state == 0 && g_ui_elements_array[i].request_render == 0) {
                        update_button(&g_ui_elements_array[i]);
                        send_expose_event();
                    }
                }
            }
            else if (g_ui_elements_array[i].enabled && g_ui_elements_array[i].type == DSTUDIO_UI_ELEMENT_TYPE_BUTTON) {
                get_pointer_coordinates(&pointer_x, &pointer_y);
                if (pointer_x >= g_ui_elements_array[i].areas.min_area_x && pointer_x <= g_ui_elements_array[i].areas.max_area_x &&
                    pointer_y >= g_ui_elements_array[i].areas.min_area_y && pointer_y <= g_ui_elements_array[i].areas.max_area_y) {
                    texture_index = 1;
                }
                else {
                    texture_index = 0;
                }

                if (texture_index != g_ui_elements_array[i].texture_index) {
                    update_button(&g_ui_elements_array[i]);
                    send_expose_event();
                }
            }
        }
        sem_post(&g_buttons_management.thread_control.mutex);
    }
    return NULL;
}

void init_buttons_management()
{
    g_buttons_management.thread_control.ready = 1;
    sem_init(&g_buttons_management.thread_control.mutex, 0, 1);
}
 
void update_button(UIElements * buttons_p) {
    buttons_p->timestamp = get_timestamp();
    if (buttons_p->texture_index == 1) {
        buttons_p->texture_index = 0;
    }
    else {
        buttons_p->texture_index = 1;
    }
    buttons_p->timestamp = get_timestamp();
    buttons_p->request_render = 1;
}
