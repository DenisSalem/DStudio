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

void configure_ui_element(UIElements * ui_elements, UIElementSettingParams * params) {
    UIElementSetting * configure_ui_element_p;
    UIElementSetting * configure_ui_element_array = params->settings;
    UIArea * ui_areas = params->areas;
    UICallback * ui_callbacks = params->callbacks;
    
    unsigned int array_offset = params->array_offset;
    printf("count %d\n", ui_elements->count);
    for (int i = 0; i < ui_elements->count; i++) {
        printf("%d\n", i);
        configure_ui_element_p = &configure_ui_element_array[array_offset+i];
        //DSTUDIO_INIT_KNOB(&sample_knobs, i, gl_x, gl_y, array_offset+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
        if (ui_elements->interactive) {
            ( (Vec2 *) ui_elements->instance_offsets_buffer)[i].x = configure_ui_element_p->gl_x;
            ( (Vec2 *) ui_elements->instance_offsets_buffer)[i].y = configure_ui_element_p->gl_y;
        }
        else {
            ( (Vec4 *) ui_elements->instance_offsets_buffer)[i].x = configure_ui_element_p->gl_x;
            ( (Vec4 *) ui_elements->instance_offsets_buffer)[i].y = configure_ui_element_p->gl_y;
        }
        ui_elements->instance_motions_buffer[i] = 0;

        DSTUDIO_SET_AREA(
            array_offset+i,
            configure_ui_element_p->min_area_x,
            configure_ui_element_p->max_area_x,
            configure_ui_element_p->min_area_y,
            configure_ui_element_p->max_area_y
        )
        DSTUDIO_SET_UI_CALLBACK(
            array_offset+i,
            params->update_callback,
            i,
            ui_elements,
            configure_ui_element_p->ui_element_type
        );
    
    }
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

void finalize_ui_element( int count, GLuint * instance_offsets_p, Vec2 * instance_offsets_buffer, GLuint * instance_motions_p, GLfloat * instance_motions_buffer, GLuint * vertex_array_object_p, GLuint vertex_buffer_object) {
    gen_gl_buffer(GL_ARRAY_BUFFER, instance_offsets_p, instance_offsets_buffer, GL_STATIC_DRAW, sizeof(Vec2) * count);
    gen_gl_buffer(GL_ARRAY_BUFFER, instance_motions_p, instance_motions_buffer, GL_DYNAMIC_DRAW, sizeof(GLfloat) * count);
    setup_vertex_array_gpu_side(vertex_array_object_p, vertex_buffer_object, *instance_offsets_p, *instance_motions_p);
}

void gen_gl_buffer(GLenum type, GLuint * buffer_object_p, void * data, GLenum mode, unsigned int data_size) {
    *buffer_object_p = 0;
    glGenBuffers(1, buffer_object_p);
    glBindBuffer(type, *buffer_object_p);
        glBufferData(type, data_size, data, mode);
    glBindBuffer(type, 0);
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

void init_background_element(
    GLchar * vertex_indexes,
    Vec4 * vertex_attributes,
    GLuint * index_buffer_object_p,
    GLuint * vertex_buffer_object_p,
    const char * texture_filename,
    unsigned char ** texture_p,
    int alpha,
    GLuint * texture_id_p,
    GLuint * vertex_array_object_p,
    GLuint texture_width,
    GLuint texture_height,
    Vec2 * scale_matrix,
    GLuint * instance_offsets_p,
    Vec4 * instance_offsets_buffer,
    GLuint count
) {
    
    DSTUDIO_SET_VERTEX_INDEXES
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES(1.0f, 1.0f)

    scale_matrix[0].x = ((float) texture_width) / ((float) DSTUDIO_VIEWPORT_WIDTH);
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_height) / ((float) DSTUDIO_VIEWPORT_HEIGHT);

    gen_gl_buffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_p, vertex_indexes, GL_STATIC_DRAW, sizeof(GLchar) * 4);
    gen_gl_buffer(GL_ARRAY_BUFFER, vertex_buffer_object_p, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);
    gen_gl_buffer(GL_ARRAY_BUFFER, instance_offsets_p, instance_offsets_buffer, GL_STATIC_DRAW, sizeof(Vec4) * count);

    get_png_pixel(texture_filename, texture_p, alpha ? PNG_FORMAT_RGBA : PNG_FORMAT_RGB);

    setup_texture_gpu_side(
        0,
        alpha,
        texture_id_p,
        texture_width,
        texture_height,
        *texture_p
    );

    setup_vertex_array_gpu_side(vertex_array_object_p, *vertex_buffer_object_p, *instance_offsets_p, 0);
}

void init_ui_elements(UIElements * ui_elements, GLuint texture_id, int interactive, unsigned int count, void (*configure_ui_element)(UIElements * ui_elements, UIElementSettingParams * params), void * params) {
    /* How many elements this group holds? */
    ui_elements->count = count;
    
    /* Copy texture identifier */
    ui_elements->texture_id = texture_id;
    
    /* Setting vertex indexes */
    GLchar * vertex_indexes = ui_elements->vertex_indexes;
    vertex_indexes[0] = 0;
    vertex_indexes[1] = 1;
    vertex_indexes[2] = 2;
    vertex_indexes[3] = 3;
    gen_gl_buffer(GL_ELEMENT_ARRAY_BUFFER, &ui_elements->index_buffer_object, vertex_indexes, GL_STATIC_DRAW, sizeof(GLchar) * 4);
    
    /* Setting default vertex coordinates */
    Vec4 * vertex_attributes = ui_elements->vertex_attributes;
    vertex_attributes[0].x = -1.0;
    vertex_attributes[0].y = 1.0;
    vertex_attributes[1].x = -1.0;
    vertex_attributes[1].y = -1.0;
    vertex_attributes[2].x =  1.0;
    vertex_attributes[2].y =  1.0;
    vertex_attributes[3].x =  1.0;
    vertex_attributes[3].y = -1.0;
    
    /* Setting default texture coordinates */
    
    //~ vertex_attributes[0].z = 0.0f;
    //~ vertex_attributes[0].w = 0.0f;
    //~ vertex_attributes[1].z = 0.0f;
    vertex_attributes[1].w = 1.0;
    vertex_attributes[2].z = 1.0;
    //~ vertex_attributes[2].w = 0.0f;
    vertex_attributes[3].z = 1.0;
    vertex_attributes[3].w = 1.0;

    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->vertex_buffer_object, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);
    
    /* Setting instance buffers */
    ui_elements->instance_offsets_buffer = malloc(count * (interactive ? sizeof(Vec2) : sizeof(Vec4)));
    explicit_bzero(ui_elements->instance_offsets_buffer, count * (interactive ? sizeof(Vec2) : sizeof(Vec4)));
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_offsets, ui_elements->instance_offsets_buffer, GL_STATIC_DRAW, count * (interactive ? sizeof(Vec2) : sizeof(Vec4)));

    if (interactive) {
        printf("interactive %d\n", interactive);
        ui_elements->instance_motions_buffer = malloc(count * sizeof(GLfloat));
        printf("BEFORE\n");
        configure_ui_element(ui_elements, params);
        printf("AFTER\n");
        gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_motions, ui_elements->instance_motions_buffer, GL_DYNAMIC_DRAW, sizeof(GLfloat) * count);
    }
        printf("THERE\n");

    /* Setting vertex array */
    glGenVertexArrays(1, &ui_elements->vertex_array_object);
    glBindVertexArray(ui_elements->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, ui_elements->vertex_buffer_object);         
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribDivisor(0, 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            glVertexAttribDivisor(1, 0);
        glBindBuffer(GL_ARRAY_BUFFER, ui_elements->instance_offsets);
            // If there is no motions required, it means that we might want to offset texture coordinates instead */
            glVertexAttribPointer(2, interactive ? 2 : 4, GL_FLOAT, GL_FALSE, interactive ? sizeof(Vec2) : sizeof(Vec4), (GLvoid *) 0 );
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
            if (interactive) {
                glBindBuffer(GL_ARRAY_BUFFER, ui_elements->instance_motions);
                    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *) 0 );
                    glEnableVertexAttribArray(3);
                    glVertexAttribDivisor(3, 1);
            }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init_ui_element(GLfloat * instance_offset_p, float offset_x, float offset_y, GLfloat * motion_buffer) {
    instance_offset_p[0] = offset_x;
    instance_offset_p[1] = offset_y;
    *motion_buffer = 0;
}

void init_ui_elements_cpu_side(int count, int * count_p, GLuint texture_scale, GLuint * texture_scale_p, const char * texture_filename, unsigned char ** texture_p, Vec2 ** offsets_buffer_p, GLfloat ** motions_buffer_p, GLchar * vertex_indexes, Vec2 * scale_matrix) {
    *count_p = count;
    *texture_scale_p = texture_scale;

    get_png_pixel(texture_filename, texture_p, PNG_FORMAT_RGBA);

    *offsets_buffer_p = malloc(count * sizeof(Vec2));
    *motions_buffer_p = malloc(count * sizeof(GLfloat));
    
    
    DSTUDIO_SET_VERTEX_INDEXES
    
    scale_matrix[0].x = ((float) texture_scale) / ((float) DSTUDIO_VIEWPORT_WIDTH);
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_scale) / ((float) DSTUDIO_VIEWPORT_HEIGHT);
}

void init_ui_elements_gpu_side(int enable_aa, Vec4 * vertex_attributes, GLuint * vertex_buffer_object_p, GLuint * texture_id_p, GLuint texture_width, GLuint texture_height, unsigned char * texture, GLuint * index_buffer_object_p, GLchar * vertex_indexes) {
    DSTUDIO_SET_VERTEX_ATTRIBUTES
    DSTUDIO_SET_S_T_COORDINATES(1.0f, 1.0f)

    gen_gl_buffer(GL_ARRAY_BUFFER, vertex_buffer_object_p, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);

    setup_texture_gpu_side(
        enable_aa,
        1,
        texture_id_p,
        texture_width,
        texture_height,
        texture
    );
    
    gen_gl_buffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_p, vertex_indexes, GL_STATIC_DRAW, sizeof(GLchar) * 4);
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

void render_ui_elements(GLuint texture_id, GLuint vertex_array_object, GLuint index_buffer_object, int count) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
        glBindVertexArray(vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
                printf("Bind element\n");
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, count);
                printf("Draw\n");
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void setup_texture_gpu_side(int enable_aa, int alpha, GLuint * texture_id_p, GLuint texture_width, GLuint texture_height, unsigned char * texture) {
    glGenTextures(1, texture_id_p);
    glBindTexture(GL_TEXTURE_2D, *texture_id_p);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, texture_width, texture_height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enable_aa ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enable_aa ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST );
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(texture);
}

GLuint setup_texture_n_scale_matrix(int enable_aa, int alpha, GLuint texture_width, GLuint texture_height, const char * texture_filename, Vec2 * scale_matrix) {
    GLuint texture_id = 0;
    unsigned char * texture_data = 0;
    get_png_pixel(texture_filename, &texture_data, alpha ? PNG_FORMAT_RGBA : PNG_FORMAT_RGB);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, texture_width, texture_height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enable_aa ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enable_aa ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST );
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(texture_data);
    
    /* Setting scale matrix if not NULL */
    if (scale_matrix) {
        scale_matrix[0].x = ((float) texture_width) / ((float) DSTUDIO_VIEWPORT_WIDTH);
        scale_matrix[0].y = 0;
        scale_matrix[1].x = 0;
        scale_matrix[1].y = ((float) texture_height) / ((float) DSTUDIO_VIEWPORT_HEIGHT);
    }
    
    return texture_id;
}

void setup_vertex_array_gpu_side(GLuint * vertex_array_object, GLuint vertex_buffer_object, GLuint instance_offsets, GLuint instance_motions) {
    glGenVertexArrays(1, vertex_array_object);
    glBindVertexArray(*vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);         
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribDivisor(0, 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            glVertexAttribDivisor(1, 0);
        glBindBuffer(GL_ARRAY_BUFFER, instance_offsets);
            glVertexAttribPointer(2, instance_motions ? 2 : 4, GL_FLOAT, GL_FALSE, instance_motions ? sizeof(Vec2) : sizeof(Vec4), (GLvoid *) 0 );
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
            if (instance_motions) {
                glBindBuffer(GL_ARRAY_BUFFER, instance_motions);
                    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *) 0 );
                    glEnableVertexAttribArray(3);
                    glVertexAttribDivisor(3, 1);
            }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
