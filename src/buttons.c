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
    
    while(!g_buttons_management.thread_control.ready) {
        usleep(1000);
    }
    
    while(!g_buttons_management.thread_control.cut_thread) {
        usleep(20000);
        sem_wait(&g_buttons_management.thread_control.mutex);
        for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) {
            timestamp = g_ui_elements_array[i].timestamp;
            if (timestamp == 0) {
                continue;
            }
            elapsed_time = get_timestamp() - g_ui_elements_array[i].timestamp;
            
            if (elapsed_time > 0.025 && g_ui_elements_array[i].type == DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE) {
                send_expose_event();
            }
        }
        sem_post(&g_buttons_management.thread_control.mutex);
    }
    return NULL;
}

void check_for_buttons_to_update() {
    sem_wait(&g_buttons_management.thread_control.mutex);
    for (unsigned int i=0; i < g_dstudio_ui_element_count; i++) {
        if (g_ui_elements_array[i].timestamp != 0) {
            if (g_ui_elements_array[i].texture_index == 1) {
                if(g_dstudio_mouse_state == 0) {
                    update_button(&g_ui_elements_array[i]);
                }
            }
            else {
                g_ui_elements_array[i].timestamp = 0;
            }
        }
    }
    sem_post(&g_buttons_management.thread_control.mutex);
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
    buttons_p->render = 1;
}
