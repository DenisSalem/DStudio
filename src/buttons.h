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

#ifndef DSTUDIO_BUTTONS_H_INCLUDED
#define DSTUDIO_BUTTONS_H_INCLUDED

#include <pthread.h>
#include <semaphore.h>

#include "ui.h"
//~ #include "text_pointer.h"
//~ #include "interactive_list.h"

typedef struct ButtonsManagement_t {
    ThreadControl thread_control;
} ButtonsManagement;

void * buttons_management_thread(void * args);
    
void init_buttons_management();

void update_button(UIElements * button_p);

extern ButtonsManagement g_buttons_management;
#endif
