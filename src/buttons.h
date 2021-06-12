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

#ifndef DSTUDIO_BUTTONS_H_INCLUDED
#define DSTUDIO_BUTTONS_H_INCLUDED

#include <pthread.h>
#include <semaphore.h>

#include "ui.h"

extern uint_fast32_t g_dstudio_buttons_count;
extern UIElements ** g_dstudio_buttons_register;

void dstudio_register_buttons();

void dstudio_update_ui_bouncing_buttons();
    
void dstudio_update_button(UIElements * button_p);

#endif
