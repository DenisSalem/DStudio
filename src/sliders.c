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

#include <stdlib.h>
#include "extensions.h"

#include "sliders.h"

void finalize_sliders(UISliders * sliders) {
    finalize_ui_element(
        sliders->count,
        &sliders->instance_offsets,
        sliders->instance_offsets_buffer,
        &sliders->instance_translations,
        sliders->instance_translations_buffer,
        &sliders->vertex_array_object,
        sliders->vertex_buffer_object
    );
}

void init_slider(UISliders * sliders, int index, float offset_x, float offset_y) {    
    init_ui_element(&sliders->instance_offsets_buffer[index], offset_x, offset_y, &sliders->instance_translations_buffer[index]);
}
