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

void free_sliders(UISliders * sliders) {
    glDeleteBuffers(1, &sliders->index_buffer_object);
    glDeleteBuffers(1, &sliders->vertex_buffer_object);
    glDeleteVertexArrays(1, &sliders->vertex_array_object);
    glDeleteTextures(1, &sliders->texture_id);
    free(sliders->instance_offsets_buffer);
    free(sliders->texture);
}

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
