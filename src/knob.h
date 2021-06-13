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

#ifndef DSTUDIO_KNOB_H_INCLUDED
#define DSTUDIO_KNOB_H_INCLUDED

#include "ui.h"

#define PI (GLfloat) 3.1415926535897932
#define KNOB_LOWEST_POSITION -2.356194490192345
#define KNOB_HIGHEST_POSITION 2.356194490192345

void bind_and_update_ui_knob(UIElements * knob, void * callback_args);
void update_knob_value_from_motion(ControllerValue * knob_value, GLfloat motion);
void update_knob_value_from_ui_element(UIElements * knob);
void update_ui_knob(UIElements * knob);

#endif
