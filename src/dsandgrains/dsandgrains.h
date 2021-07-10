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



#ifndef DSANDGRAINS_DSANDGRAINS_H_INCLUDED
#define DSANDGRAINS_DSANDGRAINS_H_INCLUDED

#include "../dstudio.h"

#include "add.h"
#include "samples.h"
#include "instances.h"
#include "samples.h"
#include "sample_screen.h"
#include "ui.h"

void dsandgrains_audio_process(VoiceContext * voice, float * out_left, float * out_right, uint_fast32_t frame_size);

#endif
