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

update_as_waveform(UIElements * bar_plot, SharedSample * shared_sample) {
    unsigned int sub_sample_size = 0;
    float positive_average = 0;
    float positive_count = 0;
    float negative_average = 0;
    float negative_count = 0;

    if (shared_sample->size > bar_plot->count) {
        sub_sample_size = shared_sample->size / bar_plot->count;
        
        for (unsigned int i = 0; i < shared_sample->size; i++) {
            if ((i != 0) && (i % sub_sample_size == 0)) {
                positive_average = 0;
                positive_count = 0;
                negative_average = 0;
                negative_count = 0;
            }
        }
    }
}
