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

#ifndef DSTUDIO_SLIDERS_H_INCLUDED
#define DSTUDIO_SLIDERS_H_INCLUDED

#include "ui.h"

float compute_slider_percentage_value(int ypos);

float compute_slider_translation(int ypos);

void compute_slider_scissor_y(
    UIElements * slider
);

void compute_slider_in_motion_scissor_y(
    UIElements * slider
);

#endif
