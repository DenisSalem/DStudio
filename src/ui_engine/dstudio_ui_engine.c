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
 *
 */
#include <png.h>
#include "dstudio_ui_engine.h"

void dstudio_compile_shader(
    GLuint shader_id, 
    GLchar ** source_pointer
) {
    glShaderSource(shader_id, 1, (const GLchar**) source_pointer , NULL);
    #ifdef DSTUDIO_DEBUG 
    printf("glShaderSource error: %d\n", glGetError()); 
    #endif
    glCompileShader(shader_id);
    #ifdef DSTUDIO_DEBUG
        printf("glCompileShader error: %d\n", glGetError());
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


static GLuint dstudio_create_shader(GLenum type) {
    GLchar * shader_buffer = NULL;
    GLuint shader_id = glCreateShader(type);
    dstudio_load_shader(&shader_buffer, type == GL_VERTEX_SHADER ? DSTUDIO_VERTEX_SHADER_PATH : DSTUDIO_FRAGMENT_SHADER_PATH);
    dstudio_compile_shader(shader_id, &shader_buffer);
    dstudio_free(shader_buffer);
    return shader_id;
}

GLuint dstudio_create_shader_program() {
    GLuint shader_program_id = 0;
    GLuint vertex_shader = dstudio_create_shader(GL_VERTEX_SHADER);
    GLuint fragment_shader =  dstudio_create_shader(GL_FRAGMENT_SHADER);

    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader);
    glAttachShader(shader_program_id, fragment_shader);
    glLinkProgram(shader_program_id);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    #ifdef DSTUDIO_DEBUG
    GLint info_log_length = 2048;
    char program_error_message[2048] = {0};

    glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    glGetProgramInfoLog(shader_program_id, info_log_length, NULL, program_error_message);

    if (strlen(program_error_message) != 0) {
        printf("%s\n", program_error_message);
    }
    #endif
    return shader_program_id;
}

GLuint dstudio_create_gl_buffer(
    GLenum type,
    void * data,
    GLenum mode,
    uint_fast32_t data_size
) {
    GLuint buffer_object = 0;
    glGenBuffers(1, &buffer_object);
    glBindBuffer(type, buffer_object);
        glBufferData(type, data_size, data, mode);
    glBindBuffer(type, 0);
    return buffer_object;
}

//~ GLuint dstudio_create_texture(uint_fast32_t flags, const char * filename) {
    //~ unsigned int texture;
    //~ glGenTextures(1, &texture);
    //~ glBindTexture(GL_TEXTURE_2D, texture);
    //~ uint_fast32_t alpha = flags & DSTUDIO_FLAG_USE_ALPHA;
    //~ uint_fast32_t enable_aa = flags & DSTUDIO_FLAG_USE_ANTI_ALIASING;
    //~ uint_fast32_t texture_is_pattern = flags & DSTUDIO_FLAG_TEXTURE_IS_PATTERN;
    //~ uint_fast8_t * texture_data = 0;
    
    //~ glGenTextures(1, &texture_id);
    //~ glBindTexture(GL_TEXTURE_2D, texture_id);
        //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_is_pattern ? GL_REPEAT: GL_CLAMP_TO_EDGE);
        //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_is_pattern ? GL_REPEAT: GL_CLAMP_TO_EDGE);
        //~ glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, texture_width, texture_height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture_data);
        //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enable_aa ? GL_LINEAR : GL_NEAREST);
        //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enable_aa ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST );
        //~ glGenerateMipmap(GL_TEXTURE_2D);
    //~ glBindTexture(GL_TEXTURE_2D, 0);
    //~ dstudio_free(texture_data);
//~ }

DStudioImage dsudio_get_png_pixels(
    const char * filename
) {
    DStudioImage output = {0};
    png_image image = {0};
    image.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&image, filename) != 0) {
        uint_fast8_t * buffer = dstudio_alloc(
            PNG_IMAGE_SIZE(image),
            DSTUDIO_FAILURE_IS_FATAL
        );
        if (buffer != NULL && png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0) {
            return output;
        }
    }
    else {
        printf("Can't load asset \"%s\": %s.\n", filename, image.message);
        exit(-1);
    }
    printf("Something went wrong while reading \"%s\".\n", filename);
    exit(-1);
}

void dstudio_load_shader(
    GLchar ** shader_buffer,
    const char * filename
) {
    FILE * shader = fopen (filename, "r");
    if (shader == NULL) {
        printf("Failed to open \"%s\" with errno: %d.\n", filename, errno);
        exit(-1);
    }
    (*shader_buffer) = dstudio_alloc(
        4096 * sizeof(GLchar),
        DSTUDIO_FAILURE_IS_FATAL
    );
    GLchar * local_shader_buffer = (*shader_buffer);
    for (uint_fast32_t i=0; i < 4096; i++) {
        local_shader_buffer[i] = (GLchar ) fgetc(shader);
        if (local_shader_buffer[i] == EOF) {
            local_shader_buffer[i] = '\0';
            break;
        }
    }
    fclose(shader);
}

DStudioImage dsudio_read_png(const char * filename) {
    DStudioImage image = {0};
    uint_fast8_t header[8] = {0};
    FILE *fp = fopen(filename, "rb");
   
    // TODO WITH DEBUG MODE INTRODUCE Custom error handling for end user
    if (!fp) {
        DSTUDIO_EXIT_IF_FAILURE(fp)
    }

    
    uint_fast8_t readed = fread(header, 1, 8, fp);
    if ( readed != 8) {
        DSTUDIO_EXIT_IF_FAILURE(readed);
    }

    uint_fast8_t is_png = (png_sig_cmp(header, 0, 8) == 0);
    if (!is_png) {
        DSTUDIO_EXIT_IF_FAILURE(is_png)
    }
   
    /* TODO
    *     If you want to use your own memory allocation routines, use a
    * libpng that was built with PNG_USER_MEM_SUPPORTED defined, and use
    * png_create_read_struct_2() instead opng_create_read_struct():
    *            png_structp png_ptr = png_create_read_struct_2(
    *                  PNG_LIBPNG_VER_STRING,
    *                  (png_voidp) user_error_ptr,
    *                  user_error_fn,
    *                  user_warning_fn,
    *                  (png_voidp) user_mem_ptr,
    *                  user_malloc_fn,
    *                  user_free_fn
    *            );
    */
    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        DSTUDIO_EXIT_IF_FAILURE(png_ptr)
    }
   
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        DSTUDIO_EXIT_IF_FAILURE(info_ptr)
    }
   
    png_set_sig_bytes(png_ptr, 8);
    png_init_io(png_ptr, fp);

    png_set_keep_unknown_chunks(png_ptr, 1, NULL, 0);

    png_set_alpha_mode(png_ptr, PNG_ALPHA_PNG, PNG_DEFAULT_sRGB);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_bytep * row_pointers = png_get_rows(png_ptr, info_ptr);
    (void) row_pointers;
    printf("DEBUG %d %d %d\n", png_get_image_width(png_ptr, info_ptr), png_get_image_height(png_ptr, info_ptr), png_get_channels(png_ptr, info_ptr));
    return image;
}
