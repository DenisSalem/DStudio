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

#include "common.h"
#include "extensions.h"
#include "ui.h"

void compile_shader(
    GLuint shader_id, 
    GLchar ** source_pointer
) {
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
/*
void configure_ui_element(
    UIElements * ui_elements,
    void * params
) {
    UIElementSettingParams * ui_element_setting_params = (UIElementSettingParams *) params;
    UIElementSetting * configure_ui_element_p;
    UIElementSetting * configure_ui_element_array = ui_element_setting_params->settings;
    UIArea * ui_areas = ui_element_setting_params->areas;
    UIArea * ui_area = 0;
    UICallback * ui_callbacks = ui_element_setting_params->callbacks;
    UICallback * ui_callback = 0;
    
    unsigned int array_offset = ui_element_setting_params->array_offset;
    
    for (unsigned int i = 0; i < ui_elements->count; i++) {
        configure_ui_element_p = &configure_ui_element_array[i];
        if (ui_elements->animated) {
            ( (Vec2 *) ui_elements->instance_offsets_buffer)[i].x = configure_ui_element_p->gl_x;
            ( (Vec2 *) ui_elements->instance_offsets_buffer)[i].y = configure_ui_element_p->gl_y;
            ui_elements->instance_motions_buffer[i] = 0;
        }
        else {
            ( (Vec4 *) ui_elements->instance_offsets_buffer)[i].x = configure_ui_element_p->gl_x;
            ( (Vec4 *) ui_elements->instance_offsets_buffer)[i].y = configure_ui_element_p->gl_y;
        }
        
        ui_area = &ui_areas[array_offset+i];
        ui_area->min_x = configure_ui_element_p->min_area_x;
        ui_area->max_x = configure_ui_element_p->max_area_x;
        ui_area->min_y = configure_ui_element_p->min_area_y;
        ui_area->max_y = configure_ui_element_p->max_area_y;
        ui_area->x     = (configure_ui_element_p->min_area_x + configure_ui_element_p->max_area_x ) / 2;
        ui_area->y     = (configure_ui_element_p->min_area_y + configure_ui_element_p->max_area_y ) / 2;
    
        ui_callback = &ui_callbacks[array_offset+i];
        ui_callback->callback = ui_element_setting_params->update_callback;
        ui_callback->index = i;
        ui_callback->context_p = ui_elements;
        ui_callback->type = configure_ui_element_p->ui_element_type;
    }
}
*/

void create_shader_program(
    GLuint * shader_program_id
) {
    GLchar * shader_buffer = NULL;
    
    // TODO Replace with create_shader function, returning shader id;
    // VERTEX SHADER
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, DSTUDIO_NON_INTERACTIVE_VERTEX_SHADER_PATH);
    compile_shader(vertex_shader, &shader_buffer);
    dstudio_free(shader_buffer);

    //FRAGMENT SHADER
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    load_shader(&shader_buffer, DSTUDIO_FRAGMENT_SHADER_PATH);
    compile_shader(fragment_shader, &shader_buffer);
    dstudio_free(shader_buffer);

    // Linking Shader

    *shader_program_id = glCreateProgram();
    glAttachShader(*shader_program_id, vertex_shader);
    glAttachShader(*shader_program_id, fragment_shader);
    glLinkProgram(*shader_program_id);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    #ifdef DSTUDIO_DEBUG
    int info_log_length = 2048;
    char program_error_message[2048] = {0};

    glGetProgramiv(*shader_program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    glGetProgramInfoLog(*shader_program_id, info_log_length, NULL, program_error_message);

    if (strlen(program_error_message) != 0) {
        printf("%s\n", program_error_message);
    }
    #endif
}

void manage_cursor_position(int xpos, int ypos) {
    (void) xpos;
    (void) ypos;
    /*float motion;
    if (active_ui_element.callback == NULL) {
        return;
    }
    if (active_ui_element.type == DSTUDIO_KNOB_TYPE_CONTINUE) {
        motion = compute_knob_rotation(xpos, ypos);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &motion);
    }
    else if (active_ui_element.type == DSTUDIO_KNOB_TYPE_DISCRETE) {
        motion = compute_knob_rotation(xpos, ypos);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &motion);
    }
    else if (active_ui_element.type == DSTUDIO_SLIDER_TYPE_VERTICAL) {
        motion = compute_slider_translation(ypos);
        active_ui_element.callback(active_ui_element.index, active_ui_element.context_p, &motion);
    }*/
}

void gen_gl_buffer(
    GLenum type,
    GLuint * buffer_object_p,
    void * data,
    GLenum mode,
    unsigned int data_size
) {
    *buffer_object_p = 0;
    glGenBuffers(1, buffer_object_p);
    glBindBuffer(type, *buffer_object_p);
        glBufferData(type, data_size, data, mode);
    glBindBuffer(type, 0);
}

int get_png_pixel(
    const char * filename,
    png_bytep * buffer,
    png_uint_32 format
) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&image, filename) != 0) {
        image.format = format;
        *buffer = dstudio_alloc(PNG_IMAGE_SIZE(image));
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

void manage_mouse_button(int xpos, int ypos, int button, int action) {
    (void) xpos;
    (void) ypos;
    (void) button;
    (void) action;
}

/*
void init_ui_element(
    GLfloat * instance_offset_p,
    float offset_x,
    float offset_y,
    GLfloat * motion_buffer
) {
    instance_offset_p[0] = offset_x;
    instance_offset_p[1] = offset_y;
    *motion_buffer = 0;
}
*/

void init_ui_elements(
    int flags,
    UIElements * ui_elements
) {
    int flip_y =  (flags & DSTUDIO_FLAG_FLIP_Y) >> 1;
    
    // Setting vertex indexes
    GLchar * vertex_indexes = ui_elements->vertex_indexes;
    vertex_indexes[0] = 0;
    vertex_indexes[1] = 1;
    vertex_indexes[2] = 2;
    vertex_indexes[3] = 3;
    gen_gl_buffer(GL_ELEMENT_ARRAY_BUFFER, &ui_elements->index_buffer_object, vertex_indexes, GL_STATIC_DRAW, sizeof(GLchar) * 4);
    
    // Setting default vertex coordinates
    Vec4 * vertex_attributes = ui_elements->vertex_attributes;
    vertex_attributes[0].x = -1.0;
    vertex_attributes[0].y =  1.0;
    vertex_attributes[1].x = -1.0;
    vertex_attributes[1].y = -1.0;
    vertex_attributes[2].x =  1.0;
    vertex_attributes[2].y =  1.0;
    vertex_attributes[3].x =  1.0;
    vertex_attributes[3].y = -1.0;
    
    // Setting default texture coordinates
    vertex_attributes[2].z = 1.0;
    vertex_attributes[3].z = 1.0;
    if (flip_y) {
        vertex_attributes[0].w = 1.0f;
        vertex_attributes[2].w = 1.0;
    }
    else {
        vertex_attributes[1].w = 1.0;
        vertex_attributes[3].w = 1.0;
    }
    
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->vertex_buffer_object, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_motions, ui_elements->instance_motions_buffer, GL_DYNAMIC_DRAW, sizeof(GLfloat) * ui_elements->count);
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_offsets, ui_elements->instance_offsets_buffer, GL_STATIC_DRAW, ui_elements->count * sizeof(Vec4));

    // Setting vertex array
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
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), (GLvoid *) 0 );
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
        glBindBuffer(GL_ARRAY_BUFFER, ui_elements->instance_motions);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *) 0 );
            glEnableVertexAttribArray(3);
            glVertexAttribDivisor(3, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/*
void init_ui_elements_settings(
    UIElementSetting ** settings_p,
    GLfloat gl_x,
    GLfloat gl_y,
    GLfloat scale_area_x,
    GLfloat scale_area_y,
    GLfloat offset_x,
    GLfloat offset_y,
    unsigned int rows,
    unsigned int count,
    unsigned int ui_element_type
) {
    *settings_p = malloc(count * sizeof(UIElementSetting));
    UIElementSetting * settings = *settings_p;
    GLfloat min_area_x = (1 + gl_x) * (DSTUDIO_VIEWPORT_WIDTH >> 1) - (scale_area_x / 2);
    GLfloat max_area_x = min_area_x + scale_area_x;
    GLfloat min_area_y = (1 - gl_y) * (DSTUDIO_VIEWPORT_HEIGHT >> 1) - (scale_area_y / 2);
    GLfloat max_area_y = min_area_y + scale_area_y;
    
    GLfloat area_offset_x = offset_x * (GLfloat)(DSTUDIO_VIEWPORT_WIDTH >> 1);
    GLfloat area_offset_y = offset_y * (GLfloat)(DSTUDIO_VIEWPORT_HEIGHT >> 1);
        
    for (unsigned int i = 0; i < count; i++) {
        unsigned int x = (i % rows);
        unsigned int y = (i / rows);
        GLfloat computed_area_offset_x = x * area_offset_x;
        GLfloat computed_area_offset_y = y * -area_offset_y;

        settings[i].gl_x = gl_x + x * offset_x;
        settings[i].gl_y = gl_y + y * offset_y;
        settings[i].min_area_x = min_area_x + computed_area_offset_x;
        settings[i].max_area_x = max_area_x + computed_area_offset_x;
        settings[i].min_area_y = min_area_y + computed_area_offset_y;
        settings[i].max_area_y = max_area_y + computed_area_offset_y;
        settings[i].ui_element_type = ui_element_type;
        
        #ifdef DSTUDIO_DEBUG
        printf("%lf %lf %lf %lf %f %lf %lf\n",
            area_offset_y,
            settings[i].gl_x,
            settings[i].gl_y,
            settings[i].min_area_x,
            settings[i].max_area_x,
            settings[i].min_area_y,
            settings[i].max_area_y
        );
        #endif

    }
}*/

void load_shader(
    GLchar ** shader_buffer,
    const char * filename
) {
    FILE * shader = fopen (filename, "r");
    if (shader == NULL) {
        printf("Failed to open \"%s\" with errno: %d.\n", filename, errno);
        exit(-1);
    }
    (*shader_buffer) = dstudio_alloc(2048 * sizeof(GLchar));
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


void render_ui_elements(UIElements * ui_elements) {
    glBindTexture(GL_TEXTURE_2D, ui_elements->texture_ids[ui_elements->texture_index]);
        glBindVertexArray(ui_elements->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui_elements->index_buffer_object);
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, ui_elements->count);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint setup_texture_n_scale_matrix(
    unsigned int flags,
    GLuint texture_width,
    GLuint texture_height,
    const char * texture_filename,
    Vec2 * scale_matrix
) {
    GLuint texture_id = 0;
    unsigned char * texture_data = 0;
    int alpha = flags & DSTUDIO_FLAG_USE_ALPHA;
    int enable_aa = flags & DSTUDIO_FLAG_USE_ANTI_ALIASING;
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
    dstudio_free(texture_data);
    
    /* Setting scale matrix if not NULL */
    if (scale_matrix) {
        scale_matrix[0].x = ((float) texture_width) / ((float) g_dstudio_viewport_width);
        scale_matrix[0].y = 0;
        scale_matrix[1].x = 0;
        scale_matrix[1].y = ((float) texture_height) / ((float) g_dstudio_viewport_height);
    }
    
    return texture_id;
}

/*
void update_and_render(
    sem_t * mutex,
    unsigned int * update,
    void (*update_callback)(),
    GLuint scissor_x,
    GLuint scissor_y,
    GLuint scissor_width,
    GLuint scissor_height,
    unsigned int render_flag
) {
    sem_wait(mutex);
    if (*update) {
        update_callback();
        glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
        render_viewport(render_flag);
        *update = 0;
    }
    sem_post(mutex);
}
*/

void update_ui_element_motion(
    int index,
    UIElements * ui_elements_p,
    void * args
) {
    float * motion = (float*) args;
    ui_elements_p->instance_motions_buffer[index] = *motion;
    glBindBuffer(GL_ARRAY_BUFFER, ui_elements_p->instance_motions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * ui_elements_p->count, ui_elements_p->instance_motions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
