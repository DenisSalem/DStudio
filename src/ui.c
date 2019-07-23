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

#include <errno.h>
       
#ifdef DSTUDIO_DEBUG
    #include <stdio.h>
#endif

#include "extensions.h"
#include "ui.h"

void compile_shader(GLuint shader_id, GLchar ** source_pointer) {
    glShaderSource(shader_id, 1, (const GLchar**) source_pointer , NULL);
    #ifdef DSTUDIO_DEBUG 
    printf("glShaderSource : %d\n", glGetError()); 
    #endif
    glCompileShader(shader_id);
    #ifdef DSTUDIO_DEBUG
        printf("glCompileShader : %d\n", glGetError());
    #endif
    #ifdef DSTUDIO_DEBUG
        GLsizei info_log_length = 2048;
        char shader_error_message[2048] = {0};
        glGetShaderInfoLog(shader_id, info_log_length, NULL, shader_error_message);
        if (strlen(shader_error_message) != 0) { 
            printf("%s\n", shader_error_message);
        }
    #endif
}

void create_shader_program(GLuint * interactive_program_id, GLuint * non_interactive_program_id) {
    GLchar * shader_buffer = NULL;
    // NON INTERACTIVE VERTEX SHADER
    GLuint non_interactive_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, DSTUDIO_NON_INTERACTIVE_VERTEX_SHADER_PATH);
    compile_shader(non_interactive_vertex_shader, &shader_buffer);
    free(shader_buffer);
 
     // INTERACTIVE VERTEX SHADER
    GLuint interactive_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, DSTUDIO_INTERACTIVE_VERTEX_SHADER_PATH);
    compile_shader(interactive_vertex_shader, &shader_buffer);
    free(shader_buffer);

    //FRAGMENT SHADER
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    load_shader(&shader_buffer, DSTUDIO_FRAGMENT_SHADER_PATH);
    compile_shader(fragment_shader, &shader_buffer);

    // Linking Shader
    *non_interactive_program_id = glCreateProgram();
    glAttachShader(*non_interactive_program_id, non_interactive_vertex_shader);
    glAttachShader(*non_interactive_program_id, fragment_shader);
    glLinkProgram(*non_interactive_program_id);

    *interactive_program_id = glCreateProgram();
    glAttachShader(*interactive_program_id, interactive_vertex_shader);
    glAttachShader(*interactive_program_id, fragment_shader);
    glLinkProgram(*interactive_program_id);
    
    glDeleteShader(non_interactive_vertex_shader);
    glDeleteShader(interactive_vertex_shader);
    glDeleteShader(fragment_shader);

    #ifdef DSTUDIO_DEBUG
    int info_log_length = 2048;
    char program_error_message[2048] = {0};

    glGetProgramiv(*interactive_program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    glGetProgramInfoLog(*interactive_program_id, info_log_length, NULL, program_error_message);

    if (strlen(program_error_message) != 0) {
        printf("%s\n", program_error_message);
    }
    
    #endif
}

int get_png_pixel(const char * filename, png_bytep * buffer, png_uint_32 format) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&image, filename) != 0) {
        image.format = format;
        *buffer = malloc(PNG_IMAGE_SIZE(image));
        if (*buffer != NULL && png_image_finish_read(&image, NULL, *buffer, 0, NULL) != 0) {
            return PNG_IMAGE_SIZE(image);
        }
    }
    else {
        printf("Can't load asset \"%s\": %s.\n", filename, image.message);
        exit(-1);
    }
    printf("Something went wrong while reading \"%s\".\n", filename);
    exit(-1);
}

void finalize_ui_element( int count, GLuint * instance_offsets_p, Vec2 * instance_offsets_buffer, GLuint * instance_motions_p, GLfloat * instance_motions_buffer, GLuint * vertex_array_object_p, GLuint vertex_buffer_object) {
    glGenBuffers(1, instance_offsets_p);
    glBindBuffer(GL_ARRAY_BUFFER, *instance_offsets_p);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * count, instance_offsets_buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, instance_motions_p);
    glBindBuffer(GL_ARRAY_BUFFER, *instance_motions_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count, instance_motions_buffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, vertex_array_object_p);
    glBindVertexArray(*vertex_array_object_p);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribDivisor(0, 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            glVertexAttribDivisor(1, 0);
        glBindBuffer(GL_ARRAY_BUFFER, *instance_offsets_p);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (GLvoid *) 0 );
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
        glBindBuffer(GL_ARRAY_BUFFER, *instance_motions_p);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *) 0 );
            glEnableVertexAttribArray(3);
            glVertexAttribDivisor(3, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void init_background(GLuint * vertex_indexes, Vec4 * vertexes_attributes, GLuint * index_buffer_object_p) {
    DSTUDIO_SET_VERTEX_INDEXES
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES
    
    *index_buffer_object_p = 0;
    glGenBuffers(1, index_buffer_object_p);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, * éindex_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   
    GLuint * vertex_buffer_object_p = &background->vertex_buffer_object;
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    get_png_pixel(DSANDGRAINS_BACKGROUND_ASSET_PATH, &background->texture, PNG_FORMAT_RGB);
    
    glGenTextures(1, &background->texture_id);
    glBindTexture(GL_TEXTURE_2D, background->texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, background->texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &background->vertex_array_object);
    glBindVertexArray(background->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, background->vertex_buffer_object);         
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init_ui_element(Vec2 * instance_offset_p, float offset_x, float offset_y, GLfloat * motion_buffer) {
    instance_offset_p->x = offset_x;
    instance_offset_p->y = offset_y;
    *motion_buffer = 0;
}

void init_ui_elements_cpu_side(int count, int * count_p, GLuint texture_scale, GLuint * texture_scale_p, const char * texture_filename, unsigned char ** texture_p, Vec2 ** offsets_buffer_p, GLfloat ** motions_buffer_p, GLchar * vertex_indexes, Vec2 * scale_matrix, int viewport_width, int viewport_height) {
    *count_p = count;
    *texture_scale_p = texture_scale;

    get_png_pixel(texture_filename, texture_p, PNG_FORMAT_RGBA);

    *offsets_buffer_p = malloc(count * sizeof(Vec2));
    *motions_buffer_p = malloc(count * sizeof(GLfloat));
    
    DSTUDIO_SET_VERTEX_INDEXES
    
    scale_matrix[0].x = ((float) texture_scale) / ((float) viewport_width);
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_scale) / ((float) viewport_height);
}

void init_ui_elements_gpu_side(int enable_aa, Vec4 * vertexes_attributes, GLuint * vertex_buffer_object_p, GLuint * texture_id_p, GLuint texture_scale, unsigned char * texture, GLuint * index_buffer_object_p, GLchar * vertex_indexes) {
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES

    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, texture_id_p);
    glBindTexture(GL_TEXTURE_2D, *texture_id_p);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_scale, texture_scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
        if (enable_aa) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(texture);
    
    glGenBuffers(1, index_buffer_object_p);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *index_buffer_object_p);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLchar), vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void load_shader(GLchar ** shader_buffer, const char * filename) {
    FILE * shader = fopen (filename, "r");
    if (shader == NULL) {
        printf("Failed to open \"%s\" with errno: %d.\n", filename, errno);
        exit(-1);
    }
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

void render_background_element(GLuint texture_id, GLuint vertex_array_object, GLuint index_buffer_object) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
        glBindVertexArray(vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
                glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid *) 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_ui_elements(GLuint texture_id, GLuint vertex_array_object, GLuint index_buffer_object, int count) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
        glBindVertexArray(vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, count);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
