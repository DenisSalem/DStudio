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

#ifndef DSTUDIO_SLIDERS_H_INCLUDED
#define DSTUDIO_SLIDERS_H_INCLUDED

#include "ui.h"
typedef struct UISliders_t {
    unsigned char * texture;
    Vec4            vertexes_attributes[4];
    Vec2            scale_matrix[2];
    int             count;
    Vec2 *          instance_offsets_buffer;
    GLuint          instance_offsets;
    GLfloat *       instance_translations_buffer;
    GLuint          instance_translations;
    GLuint          vertex_buffer_object;
    GLuint          vertex_array_object;
    GLchar          vertex_indexes[4];
    GLuint          texture_id;
    GLuint          texture_scale;
    GLuint          index_buffer_object;
} UISliders;

void finalize_sliders(UISliders * sliders);
void init_slider(UISliders * sliders, int index, float x, float y);
void init_sliders_cpu_side(UISliders * sliders, int count, GLuint texture_scale, const char * texture_filename);
void init_sliders_gpu_side(UISliders * sliders);
void render_sliders(UISliders * sliders);
void update_slider(int index, void * context, void * args);

#define DSTUDIO_INIT_SLIDER(sliders_p, slider_index, gl_x, gl_y, ui_element_index, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type) \
        init_slider( \
            sliders_p, \
            slider_index, \
            init_slider_array_p->gl_x, \
            init_slider_array_p->gl_y \
        ); \
        DSTUDIO_SET_AREA( \
            ui_element_index, \
            init_slider_array_p->min_area_x, \
            init_slider_array_p->max_area_x, \
            init_slider_array_p->min_area_y, \
            init_slider_array_p->max_area_y \
        ) \
        DSTUDIO_SET_UI_CALLBACK( \
            ui_element_index,\
            update_slider, \
            slider_index,\
            sliders_p, \
            init_slider_array_p->ui_element_type\
        );
#endif
