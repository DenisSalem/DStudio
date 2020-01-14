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

#include "buttons.h"
#include "extensions.h"
#include "ui.h"

GLuint g_shader_program_id = 0;
GLuint g_scale_matrix_id = 0;
GLuint g_motion_type_location = 0;

static int s_ui_element_index = -1;
static int s_ui_element_center_x = 0;
static int s_ui_element_center_y = 0;
static int s_active_slider_range_max = 0;
static int s_active_slider_range_min = 0;

static inline float compute_knob_rotation(int xpos, int ypos) {
    float rotation = 0;
    float y = - ypos + s_ui_element_center_y;
    float x = xpos - s_ui_element_center_x;
    rotation = -atan(x / y);

    if (y < 0) {
        if (x < 0) {
            rotation += 3.141592;
        }
        else {
                rotation -= 3.141592;
        }
    }
    if (rotation > 2.356194) {
        rotation = 2.356194;
    }
    else if (rotation < -2.356194) {
        rotation = -2.356194;
    }

    return rotation;
}

static inline float compute_slider_translation(int ypos) {
    if (ypos > s_active_slider_range_max) {
        ypos = s_active_slider_range_max;
    }
    else if (ypos < s_active_slider_range_min) {
        ypos = s_active_slider_range_min;
    }
    float translation = - ypos + s_ui_element_center_y;
    return translation / (g_dstudio_viewport_height >> 1);
}

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

void create_shader_program(
    GLuint * shader_program_id
) {
    GLchar * shader_buffer = NULL;
    
    // TODO Replace with create_shader function, returning shader id;
    // VERTEX SHADER
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, DSTUDIO_VERTEX_SHADER_PATH);
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

void manage_cursor_position(int xpos, int ypos) {
    float motion;
    if (s_ui_element_index < 0) {
        return;
    }
    switch(g_ui_elements_array[s_ui_element_index].type) {
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
            motion = compute_knob_rotation(xpos, ypos);
            update_ui_element_motion(&g_ui_elements_array[s_ui_element_index], motion);
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            motion = compute_slider_translation(ypos);
            update_ui_element_motion(&g_ui_elements_array[s_ui_element_index], motion);
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND:
        case DSTUDIO_UI_ELEMENT_TYPE_TEXT:
        default:
            break;
    }
}

void manage_mouse_button(int xpos, int ypos, int button, int action) {
    UIElements * ui_elements_p = 0;
    if (button == DSTUDIO_MOUSE_BUTTON_LEFT && action == DSTUDIO_MOUSE_BUTTON_PRESS) {
        for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) {
            ui_elements_p = &g_ui_elements_array[i];
            if (xpos > ui_elements_p->areas.min_area_x && xpos < ui_elements_p->areas.max_area_x && ypos > ui_elements_p->areas.min_area_y && ypos < ui_elements_p->areas.max_area_y && ui_elements_p->enabled) {
                s_ui_element_index = i;
                switch (ui_elements_p->type) {
                    case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
                    case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
                        update_button(ui_elements_p);
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
                        s_active_slider_range_min = ui_elements_p->areas.min_area_y + DSTUDIO_SLIDER_1_10_HEIGHT / 2;
                        s_active_slider_range_max = ui_elements_p->areas.max_area_y - DSTUDIO_SLIDER_1_10_HEIGHT / 2;
                        __attribute__ ((fallthrough));
                                                                   
                    case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
                        s_ui_element_center_x = (int)(ui_elements_p->areas.min_area_x + ui_elements_p->areas.max_area_x) >> 1;
                        s_ui_element_center_y = (int)(ui_elements_p->areas.min_area_y + ui_elements_p->areas.max_area_y) >> 1;
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND:
                    case DSTUDIO_UI_ELEMENT_TYPE_TEXT:
                        break;
                }
                break;
            }
        }
        return;
    }
    else if(action == DSTUDIO_MOUSE_BUTTON_RELEASE) {
        s_ui_element_index = -1;
    }
}

void init_opengl_ui_elements(
    int flags,
    UIElements * ui_elements
) {
    int flip_y = flags & DSTUDIO_FLAG_FLIP_Y;
    int text_setting = flags & DSTUDIO_FLAG_USE_TEXT_SETTING;
    
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

    if (text_setting) {
        vertex_attributes[1].w /= (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR;
        vertex_attributes[2].z /= (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR;
        vertex_attributes[3].z /= (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR;
        vertex_attributes[3].w /= (GLfloat) DSTUDIO_CHAR_SIZE_DIVISOR;
    }
    
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->vertex_buffer_object, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_motions, ui_elements->instance_motions_buffer, GL_DYNAMIC_DRAW, ui_elements->count * sizeof(GLfloat) );
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_offsets, ui_elements->instance_offsets_buffer, GL_STATIC_DRAW, ui_elements->count * sizeof(Vec4));
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_alphas, ui_elements->instance_alphas_buffer, GL_STATIC_DRAW, ui_elements->count * sizeof(GLfloat));

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
        glBindBuffer(GL_ARRAY_BUFFER, ui_elements->instance_alphas);
            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *) 0 );
            glEnableVertexAttribArray(4);
            glVertexAttribDivisor(4, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init_ui_elements(
    UIElements * ui_elements_array,
    GLuint * texture_ids,
    Vec2 * scale_matrix,
    GLfloat gl_x,
    GLfloat gl_y,
    GLfloat area_width,
    GLfloat area_height,
    GLfloat offset_x,
    GLfloat offset_y,
    unsigned int columns,
    unsigned int count,
    unsigned int instances_count,
    UIElementType ui_element_type,
    int flags
) {
    GLfloat min_area_x;
    if (ui_element_type == DSTUDIO_UI_ELEMENT_TYPE_TEXT) {
        min_area_x = (1 + gl_x - scale_matrix[0].x) * (g_dstudio_viewport_width >> 1); 
    }
    else {
        min_area_x = (1 + gl_x) * (g_dstudio_viewport_width >> 1) - (area_width / 2); 
    }
    GLfloat max_area_x = min_area_x + area_width;
    GLfloat min_area_y = (1 - gl_y) * (g_dstudio_viewport_height >> 1) - (area_height / 2);
    GLfloat max_area_y = min_area_y + area_height;
         
    GLfloat area_offset_x = offset_x * (GLfloat)(g_dstudio_viewport_width >> 1);
    GLfloat area_offset_y = offset_y * (GLfloat)(g_dstudio_viewport_height >> 1);
        
    for (unsigned int i = 0; i < count; i++) {
        unsigned int x = (i % columns);
        unsigned int y = (i / columns);
        
        GLfloat computed_area_offset_x = x * area_offset_x;
        GLfloat computed_area_offset_y = y * -area_offset_y;

        ui_elements_array[i].areas.min_area_x = min_area_x + computed_area_offset_x;
        ui_elements_array[i].areas.max_area_x = max_area_x + computed_area_offset_x;
        ui_elements_array[i].areas.min_area_y = min_area_y + computed_area_offset_y;
        ui_elements_array[i].areas.max_area_y = max_area_y + computed_area_offset_y;
        
        ui_elements_array[i].type = ui_element_type;
        
        ui_elements_array[i].instance_alphas_buffer = dstudio_alloc(sizeof(GLfloat) * instances_count);        
        ui_elements_array[i].instance_motions_buffer = dstudio_alloc(sizeof(GLfloat) * instances_count);
        ui_elements_array[i].instance_offsets_buffer = dstudio_alloc(sizeof(Vec4) * instances_count);
        
        for (unsigned int j = 0; j < instances_count; j++) {
            ui_elements_array[i].instance_alphas_buffer[j] = 1.0;
            ui_elements_array[i].instance_offsets_buffer[j].x = gl_x + (j * scale_matrix[0].x * 2) + (x * offset_x);
            ui_elements_array[i].instance_offsets_buffer[j].y = gl_y + (y * offset_y); 
        }
        
        ui_elements_array[i].scissor.x = min_area_x + computed_area_offset_x;
        ui_elements_array[i].scissor.y = g_dstudio_viewport_height - max_area_y - computed_area_offset_y;
        ui_elements_array[i].scissor.width = max_area_x - min_area_x;
        ui_elements_array[i].scissor.height = max_area_y - min_area_y;
        
        ui_elements_array[i].count = instances_count;
        
        memcpy(ui_elements_array[i].texture_ids, texture_ids, sizeof(GLuint) * 2);
        
        ui_elements_array[i].scale_matrix = scale_matrix;
        
        ui_elements_array[i].render = 1;
        
        init_opengl_ui_elements(
            (ui_element_type == DSTUDIO_UI_ELEMENT_TYPE_TEXT ? DSTUDIO_FLAG_USE_TEXT_SETTING : DSTUDIO_FLAG_NONE) | flags,
            &ui_elements_array[i]
        );
    }
}

void load_shader(
    GLchar ** shader_buffer,
    const char * filename
) {
    FILE * shader = fopen (filename, "r");
    if (shader == NULL) {
        printf("Failed to open \"%s\" with errno: %d.\n", filename, errno);
        exit(-1);
    }
    (*shader_buffer) = dstudio_alloc(4096 * sizeof(GLchar));
    GLchar * local_shader_buffer = (*shader_buffer);
    for (int i=0; i < 4096; i++) {
        local_shader_buffer[i] = (GLchar ) fgetc(shader);
        if (local_shader_buffer[i] == EOF) {
            local_shader_buffer[i] = '\0';
            break;
        }
    }
    fclose(shader);
}

void render_ui_elements(UIElements * ui_elements) {
    switch(ui_elements->type) {
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_ROTATION);
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_SLIDE);
            break;
             
        case DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND:
        case DSTUDIO_UI_ELEMENT_TYPE_TEXT:
        case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
        case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_NONE);
            break;
    }

    glBindTexture(GL_TEXTURE_2D, ui_elements->texture_ids[ui_elements->texture_index]);
        glBindVertexArray(ui_elements->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui_elements->index_buffer_object);
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, ui_elements->count);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_viewport(unsigned int render_all) {
    // UI element at index 0 is always background
    if (g_ui_elements_array[0].render || render_all) {       
        glScissor(
            g_ui_elements_array[0].scissor.x,
            g_ui_elements_array[0].scissor.y,
            g_ui_elements_array[0].scissor.width,
            g_ui_elements_array[0].scissor.height
        );
        glUniformMatrix2fv(
            g_scale_matrix_id,
            1,
            GL_FALSE,
            (float *) g_ui_elements_array[0].scale_matrix
        );
        render_ui_elements(&g_ui_elements_array[0]);
        g_ui_elements_array[0].render = 0;
    }
    else {
        for (unsigned int i = 1; i < g_dstudio_ui_element_count; i++) {
            if (g_ui_elements_array[i].render) {
                glScissor(
                    g_ui_elements_array[i].scissor.x,
                    g_ui_elements_array[i].scissor.y,
                    g_ui_elements_array[i].scissor.width,
                    g_ui_elements_array[i].scissor.height
                );
                glUniformMatrix2fv(
                    g_scale_matrix_id,
                    1,
                    GL_FALSE,
                    (float *) g_ui_elements_array[0].scale_matrix
                );
                render_ui_elements(&g_ui_elements_array[0]);
            }
        }
    }
    for (unsigned int i = 1; i < g_dstudio_ui_element_count; i++) {
        if (g_ui_elements_array[i].render || render_all) {
            glScissor(
                g_ui_elements_array[i].scissor.x,
                g_ui_elements_array[i].scissor.y,
                g_ui_elements_array[i].scissor.width,
                g_ui_elements_array[i].scissor.height
            );
            glUniformMatrix2fv(
                g_scale_matrix_id,
                1,
                GL_FALSE,
                (float *) g_ui_elements_array[i].scale_matrix
            );
            render_ui_elements(&g_ui_elements_array[i]);
            g_ui_elements_array[i].render = 0;
        }
    }
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
    int text_setting = flags & DSTUDIO_FLAG_USE_TEXT_SETTING;
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
    
    if (scale_matrix) {
        scale_matrix[0].x = (float) texture_width / (float) g_dstudio_viewport_width;
        scale_matrix[0].y = 0;
        scale_matrix[1].x = 0;
        scale_matrix[1].y = (float) texture_height / (float) g_dstudio_viewport_height;
        if (text_setting) {
            scale_matrix[0].x /= DSTUDIO_CHAR_SIZE_DIVISOR;
            scale_matrix[1].y /= DSTUDIO_CHAR_SIZE_DIVISOR;
        }
    }
    return texture_id;
}

void update_threaded_ui_element(ThreadControl * thread_control, void (*update_callback)()) {
    sem_wait(&thread_control->mutex);
    if (!thread_control->update) {
        sem_post(&thread_control->mutex);
        return;
    }
    update_callback();
    thread_control->update = 0;
    sem_post(&thread_control->mutex);
}

void update_ui_element_motion(
    UIElements * ui_elements_p,
    float motion
) {
    *ui_elements_p->instance_motions_buffer = motion;
    glBindBuffer(GL_ARRAY_BUFFER, ui_elements_p->instance_motions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * ui_elements_p->count, ui_elements_p->instance_motions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ui_elements_p->render = 1;
}
