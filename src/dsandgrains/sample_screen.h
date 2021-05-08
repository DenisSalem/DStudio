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

#include "../samples.h"

typedef enum SampleScreenRenderRequest_t {
    DSANDGRAINS_SAMPLE_SCREEN_NO_RENDER_REQUEST = 0,
    DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_DATA = 1,
    DSANDGRAINS_SAMPLE_SCREEN_RENDER_REQUEST_NEW_RANGE = 2
} SampleScreenRenderRequest;

void bind_new_data_to_sample_screen(SharedSample * shared_sample);

void update_sample_screen();

void update_sample_screen_range();

void request_sample_screen_range_update();


