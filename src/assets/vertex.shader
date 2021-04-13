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

#version 330

const uint DSTUDIO_MOTION_TYPE_NONE = 0U;
const uint DSTUDIO_MOTION_TYPE_ROTATION = 1U;
const uint DSTUDIO_MOTION_TYPE_SLIDE = 2U;
const uint DSTUDIO_MOTION_TYPE_BAR_PLOT = 3U; 

 
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec4 offset;
layout(location = 3) in float motion;
layout(location = 4) in float in_alpha;

uniform mat2            scale_matrix;
uniform uint            no_texture;
uniform uint            motion_type;

out vec2 fragment_texture_coordinates;
flat out float alpha;

void main() {
    alpha = in_alpha;
    vec2 applied_rotation; 
    float c;
    float s;
    mat2 alternate_scale_matrix = mat2(scale_matrix);
    
    if ( motion_type == DSTUDIO_MOTION_TYPE_NONE) {
        gl_Position = vec4( scale_matrix * vertex_position + offset.xy, 0, 1.0);
    }
    else if (motion_type == DSTUDIO_MOTION_TYPE_BAR_PLOT) {
        alternate_scale_matrix[1].y *= motion;
        gl_Position = vec4( alternate_scale_matrix * vertex_position + offset.xy , 0, 1.0);
    }
    else if (motion_type == DSTUDIO_MOTION_TYPE_ROTATION) {    
        if (motion != 0) {  
            c = cos(motion);
            s = sin(motion);
            applied_rotation.x = vertex_position.x * c - vertex_position.y * s;
            applied_rotation.y = vertex_position.x * s + vertex_position.y * c;
        }
        else {
            applied_rotation = vertex_position.xy;
        }
        gl_Position = vec4( scale_matrix * applied_rotation + offset.xy, 0, 1.0);
    }
    else if (motion_type == DSTUDIO_MOTION_TYPE_SLIDE) {
        gl_Position = vec4( scale_matrix * vertex_position.xy + offset.xy + vec2(0, motion), 0, 1.0);
    }
    
    if (no_texture != 1U) {
        fragment_texture_coordinates = texture_coordinates + offset.zw;
    }
} 
