/*
 * Copyright 2019, 2025 Denis Salem
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

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_TexCoord;

out vec2 TexCoord;

uniform mat2            scale_matrix;

void main() {
    gl_Position = vec4(in_position, 1);
    TexCoord = in_TexCoord;
}

