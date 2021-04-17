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

#include "../bar_plot.h"
#include "sample_screen.h"

static unsigned char s_update_request = 0;
static SharedSample * s_shared_sample = 0;

void bind_new_data(SharedSample * shared_sample) {
    s_update_request = 1;
    s_shared_sample = shared_sample;
}

void update_sample_screen() {
    if (s_update_request) {
        s_refresh_sample_screen = 0;
        update_bar_plot_as_waveform(&g_ui_elements_struct.sample_screen, s_shared_sample);
    }
}
