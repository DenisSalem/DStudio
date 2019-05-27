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

#ifdef DSTUDIO_DEBUG
    #include <stdio.h>
#endif

#include "extensions.h"
#include "ui.h"

int get_png_pixel(const char * filename, png_bytep * buffer) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&image, filename) != 0) {
        image.format = PNG_FORMAT_RGBA;
        *buffer = malloc(PNG_IMAGE_SIZE(image));
        if (*buffer != NULL && png_image_finish_read(&image, NULL, *buffer, 0, NULL) != 0) {
            return PNG_IMAGE_SIZE(image);
        }
    }
    return 0;
}

void compile_shader(GLuint shader_id, GLchar ** source_pointer) {
    glShaderSource(shader_id, 1, (const GLchar**) source_pointer , NULL);
    glCompileShader(shader_id);

    #ifdef DSTUDIO_DEBUG
        GLsizei info_log_length = 2048;
        char shader_error_message[2048] = {0};

        glGetShaderInfoLog(shader_id, info_log_length, NULL, shader_error_message);
    
        if (strlen(shader_error_message) != 0) { 
            printf("%s\n", shader_error_message);
        }
    #endif

}

void load_shader(GLchar ** shader_buffer, const char * filename) {
    FILE * shader = fopen (filename, "r");
    (*shader_buffer) = malloc(2048 * sizeof(GLchar));
    GLchar * local_shader_buffer = (*shader_buffer);
    for (int i=0; i < 2048; i++) {
        local_shader_buffer[i] = (GLchar ) fgetc(shader);
        if (local_shader_buffer[i] == EOF) {
            local_shader_buffer[i] = '\0';
            break;
        }
    }
    fclose(shader);
}
