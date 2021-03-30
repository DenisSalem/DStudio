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

#include <errno.h>
#include <unistd.h>
       
#ifdef DSTUDIO_DEBUG
    #include <stdio.h>
#endif

#include "buttons.h"
#include "extensions.h"
#include "instances.h"
#include "text_pointer.h"
#include "sliders.h"
#include "ui.h"
#include "window_management.h"

int     g_active_slider_range_max = 0;
int     g_active_slider_range_min = 0;
Vec2    g_background_scale_matrix[2] = {0};

GLuint  g_charset_8x18_texture_ids[2] = {0};
GLuint  g_charset_4x9_texture_ids[2] = {0};
Vec2    g_charset_8x18_scale_matrix[2] = {0};
Vec2    g_charset_4x9_scale_matrix[2] = {0};
GLfloat g_saved_scissor_y;
GLuint  g_shader_program_id = 0;
GLuint  g_scale_matrix_id = 0;
GLuint  g_motion_type_location = 0;
GLuint  g_no_texture_location = 0;
int     g_ui_element_center_x = 0;
int     g_ui_element_center_y = 0;
GLuint  g_ui_element_color_location = 0;

unsigned int g_framerate = DSTUDIO_FRAMERATE;
UIElements * g_menu_background_enabled = 0;
unsigned int g_menu_background_index = 0;
unsigned int g_request_render_all = 0;
WindowScale                 g_previous_window_scale = {0,0};

static Vec2                 s_framebuffer_scale_matrix[2] = {{1.0, 0.0},{0.0,-1.0}};
static GLuint               s_framebuffer_objects[DSTUDIO_FRAMEBUFFER_COUNT] = {0};
static GLuint               s_framebuffer_textures[DSTUDIO_FRAMEBUFFER_COUNT] = {0};
static UIElements           s_framebuffer_quad = {0};
static double               s_list_item_click_timestamp = 0;
static int                  s_ui_element_index = -1;
static UIElements **        s_ui_elements_requests = 0;
static unsigned int         s_ui_elements_requests_index = 0;
static UpdaterRegister *    s_updater_register = 0;
static unsigned int         s_updater_register_index = 0;
static long                 s_x11_input_mask = 0;
static int                  s_scissor_offset_x = 0;
static int                  s_scissor_offset_y = 0;

#ifdef DSTUDIO_DEBUG
static void check_frame_buffer_status() {
    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
            break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
            break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
            break;
            
        case GL_FRAMEBUFFER_UNSUPPORTED:
            printf("GL_FRAMEBUFFER_UNSUPPORTED\n");
            break;
            
        case GL_FRAMEBUFFER_COMPLETE:
            printf(" GL_FRAMEBUFFER_COMPLETE\n");
            break;
    }
}
#endif

static inline GLfloat compute_knob_rotation(int xpos, int ypos) {
    GLfloat rotation = 0;
    GLfloat y = - ypos + g_ui_element_center_y;
    GLfloat x = xpos - g_ui_element_center_x;
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

static int is_background_ui_element(UIElements * ui_element) {
    if (ui_element->type & DSTUDIO_ANY_BACKGROUND_TYPE) {
        return 1;
    }
    return 0;
}

static void render_layers(unsigned int limit) {
    for (unsigned int index = 0; index < limit; index++) {
        s_framebuffer_quad.texture_index = index;
        render_ui_elements(&s_framebuffer_quad);
    }
}

static void update_scale_matrix(Vec2 * scale_matrix);

static void scissor_n_matrix_setting(int scissor_index, int matrix_index, int flags, int scissor_offset_x, int scissor_offset_y) {
    UIElements * ui_element = s_ui_elements_requests[scissor_index];
    Scissor * scissor = &ui_element->coordinates_settings.scissor;
    UIElementType type = ui_element->type;
    int is_active_text_pointer_overing = \
        g_text_pointer_context.active && \
        g_text_pointer_context.ui_text->render_state != DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED && \
        ( \
           type == DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT || \
           type == DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM \
        ) && \
        (
            g_text_pointer_context.highlight == ui_element || \
            g_text_pointer_context.ui_text == ui_element \
        );
        
    if (scissor_index >=0) {
        glScissor(
            scissor_offset_x + scissor->x,
            scissor_offset_y + (flags & DSTUDIO_FLAG_RESET_HIGHLIGHT_AREAS ? \
                ui_element->previous_highlight_scissor_y : \
                scissor->y)
            ,
            is_active_text_pointer_overing ? 1 : scissor->width ,
            scissor->height
        );
    }
    if (!(flags & DSTUDIO_FLAG_OVERLAP)) {
        update_scale_matrix(matrix_index >= 0 ? s_ui_elements_requests[matrix_index]->coordinates_settings.scale_matrix : s_framebuffer_scale_matrix);
    }
}

static void update_scale_matrix(Vec2 * scale_matrix) {
    glUniformMatrix2fv(
        g_scale_matrix_id,
        1,
        GL_FALSE,
        (float *) scale_matrix
    );
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
        *buffer = dstudio_alloc(
            PNG_IMAGE_SIZE(image),
            DSTUDIO_FAILURE_IS_FATAL
        );
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

void init_opengl_ui_elements(
    int flags,
    UIElements * ui_elements
) {
    int flip_y = flags & DSTUDIO_FLAG_FLIP_Y;
    int text_setting = flags & DSTUDIO_FLAG_USE_TEXT_SETTING;
    int slider_background_setting = flags & DSTUDIO_FLAG_USE_SLIDER_BACKGROUND_SETTING;
    int texture_is_pattern = flags & DSTUDIO_FLAG_TEXTURE_IS_PATTERN;
    
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
        vertex_attributes[0].w = 1.0;
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
    else if (slider_background_setting) {
        vertex_attributes[1].w /= (GLfloat) 3.0;
        vertex_attributes[3].w /= (GLfloat) 3.0;
    }
    else if (texture_is_pattern) {
        vertex_attributes[1].w *= ((GLfloat) ui_elements->coordinates_settings.scale_matrix[1].y * g_dstudio_viewport_height) / DSTUDIO_PATTERN_SCALE;
        vertex_attributes[2].z *= ((GLfloat) ui_elements->coordinates_settings.scale_matrix[0].x * g_dstudio_viewport_width) / DSTUDIO_PATTERN_SCALE;
        vertex_attributes[3].z *= ((GLfloat) ui_elements->coordinates_settings.scale_matrix[0].x * g_dstudio_viewport_width) / DSTUDIO_PATTERN_SCALE;
        vertex_attributes[3].w *= ((GLfloat) ui_elements->coordinates_settings.scale_matrix[1].y * g_dstudio_viewport_height) / DSTUDIO_PATTERN_SCALE;
    }
    
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->vertex_buffer_object, vertex_attributes, GL_STATIC_DRAW, sizeof(Vec4) * 4);
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_motions, ui_elements->instance_motions_buffer, GL_DYNAMIC_DRAW, ui_elements->count * sizeof(GLfloat) );
    gen_gl_buffer(GL_ARRAY_BUFFER, &ui_elements->instance_offsets, ui_elements->coordinates_settings.instance_offsets_buffer, GL_STATIC_DRAW, ui_elements->count * sizeof(Vec4));
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

void init_text() {
    g_charset_8x18_texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_TEXT_SETTING,
        DSTUDIO_CHAR_TABLE_8X18_WIDTH,
        DSTUDIO_CHAR_TABLE_8X18_HEIGHT,
        DSTUDIO_CHAR_TABLE_8X18_ASSET_PATH,
        g_charset_8x18_scale_matrix,
        NULL
    );
    
    g_charset_4x9_texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_TEXT_SETTING,
        DSTUDIO_CHAR_TABLE_4X9_WIDTH,
        DSTUDIO_CHAR_TABLE_4X9_HEIGHT,
        DSTUDIO_CHAR_TABLE_4X9_ASSET_PATH,
        g_charset_4x9_scale_matrix,
        NULL
    );
}

void init_ui_element_updater_register(unsigned int updater_count) {
    s_updater_register = dstudio_alloc(
        updater_count * sizeof(UpdaterRegister),
        DSTUDIO_FAILURE_IS_FATAL
    );
}

void init_ui() {
    g_previous_window_scale.width = g_dstudio_viewport_width;
    g_previous_window_scale.height = g_dstudio_viewport_height;
    
    init_context(
        g_application_name,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height
    );
    
    set_mouse_button_callback(manage_mouse_button);
    set_cursor_position_callback(manage_cursor_position);
    	
    DSTUDIO_EXIT_IF_FAILURE(load_extensions())
    
    glGenFramebuffers(
        DSTUDIO_FRAMEBUFFER_COUNT,
        &s_framebuffer_objects[0]
    );
    glGenTextures(
        DSTUDIO_FRAMEBUFFER_COUNT,
        &s_framebuffer_textures[0]
    );
    for (unsigned int i = 0; i < DSTUDIO_FRAMEBUFFER_COUNT; i++) {
        glBindTexture(GL_TEXTURE_2D, s_framebuffer_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_dstudio_viewport_width, g_dstudio_viewport_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);      
        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffer_objects[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_framebuffer_textures[i], 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //DEFINE_SCALE_MATRIX(s_framebuffer_matrix, g_dstudio_viewport_width, (int) -g_dstudio_viewport_height)
    
    #ifdef DSTUDIO_DEBUG
    glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffer_objects[0]);
        check_frame_buffer_status();
    glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffer_objects[1]);
        check_frame_buffer_status();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    #endif

    init_ui_elements(
        &s_framebuffer_quad,
        &s_framebuffer_textures[0],
        (Vec2 *) &s_framebuffer_scale_matrix[0],
        0,
        0,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_LAYER,
        DSTUDIO_FLAG_USE_ALPHA
    );
    
    create_shader_program(&g_shader_program_id);
    
    g_motion_type_location = glGetUniformLocation(g_shader_program_id, "motion_type");
    g_no_texture_location = glGetUniformLocation(g_shader_program_id, "no_texture");
    g_ui_element_color_location = glGetUniformLocation(g_shader_program_id, "ui_element_color");

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);
    glUseProgram(g_shader_program_id);
    
    s_ui_elements_requests = dstudio_alloc(
        sizeof(UIElements *) * g_dstudio_ui_element_count,
        DSTUDIO_FAILURE_IS_FATAL
    );
};

// TODO: MAY HAVE ROOM FOR EDGE CASES REMOVAL AND CODE 

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
    GLfloat min_area_y;
    if (ui_element_type & (DSTUDIO_ANY_TEXT_TYPE)) {
        min_area_x = (1 + gl_x - scale_matrix[0].x) * (g_dstudio_viewport_width >> 1); 
    }
    else {
        min_area_x = (1 + gl_x) * (g_dstudio_viewport_width >> 1) - (area_width / 2); 
    }
    if (ui_element_type & DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND) {
        min_area_y = round((1 - gl_y - (scale_matrix[1].y)) * (g_dstudio_viewport_height >> 1));
    }
    else {
        min_area_y = (1 - gl_y) * (g_dstudio_viewport_height >> 1) - (area_height / 2);
    }
    
    GLfloat max_area_x = min_area_x + area_width;
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
        
        ui_elements_array[i].instance_alphas_buffer = dstudio_alloc(
            sizeof(GLfloat) * instances_count,
            DSTUDIO_FAILURE_IS_FATAL
        );        
        ui_elements_array[i].instance_motions_buffer = dstudio_alloc(
            sizeof(GLfloat) * instances_count,
            DSTUDIO_FAILURE_IS_FATAL
        );
        ui_elements_array[i].coordinates_settings.instance_offsets_buffer = dstudio_alloc(
            sizeof(Vec4) * instances_count,
            DSTUDIO_FAILURE_IS_FATAL
        );
        
        for (unsigned int j = 0; j < instances_count; j++) {
            ui_elements_array[i].instance_alphas_buffer[j] = 1.0;
            if (ui_element_type == DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND) {
                ui_elements_array[i].coordinates_settings.instance_offsets_buffer[j].x = gl_x + (x * offset_x);
                ui_elements_array[i].coordinates_settings.instance_offsets_buffer[j].y = gl_y - (j * scale_matrix[1].y * 2) + (y * offset_y);
                // Setup texture coordinates for SLIDER BACKGROUND
                if (j == instances_count-1) {
                    ui_elements_array[i].coordinates_settings.instance_offsets_buffer[j].w = 2.0/3.0;
                }
                else if (j != 0) {
                    ui_elements_array[i].coordinates_settings.instance_offsets_buffer[j].w = 1.0/3.0;
                }
            }
            else {
                ui_elements_array[i].coordinates_settings.instance_offsets_buffer[j].x = gl_x + (j * scale_matrix[0].x * 2) + (x * offset_x);
                ui_elements_array[i].coordinates_settings.instance_offsets_buffer[j].y = gl_y + (y * offset_y); 
                if (flags & DSTUDIO_FLAG_SLIDER_TO_TOP) {
                    ui_elements_array[i].instance_motions_buffer[j] = \
                        (GLfloat) (area_height) * (1.0 / (GLfloat) g_dstudio_viewport_height) - scale_matrix[1].y;
                }
            }
        }
        
        ui_elements_array[i].coordinates_settings.scale_matrix = scale_matrix;
        
        ui_elements_array[i].coordinates_settings.scissor.x = min_area_x + computed_area_offset_x;
        ui_elements_array[i].coordinates_settings.scissor.width = ui_element_type & (DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM | DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM) ? 0 : max_area_x - min_area_x;
        
        if (ui_element_type == DSTUDIO_UI_ELEMENT_TYPE_SLIDER) {
            compute_slider_scissor_y(&ui_elements_array[i]);
        }
        else {
            ui_elements_array[i].coordinates_settings.scissor.y = g_dstudio_viewport_height - max_area_y - computed_area_offset_y;
            ui_elements_array[i].coordinates_settings.scissor.height = max_area_y - min_area_y;
        }
        
        ui_elements_array[i].count = instances_count;
        
        if(texture_ids) {
            memcpy(ui_elements_array[i].texture_ids, texture_ids, sizeof(GLuint) * 2);
        }
        if (ui_element_type & (DSTUDIO_ANY_TEXT_TYPE | DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT)) {
            ui_elements_array[i].texture_index = 1;
        }
        
        ui_elements_array[i].visible = (flags & DSTUDIO_FLAG_IS_VISIBLE) != 0;
        ui_elements_array[i].render_state = ui_elements_array[i].visible ? DSTUDIO_UI_ELEMENT_RENDER_REQUESTED : DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED;
        
        
        if (ui_element_type & DSTUDIO_ANY_TEXT_TYPE) {
            flags |= DSTUDIO_FLAG_USE_TEXT_SETTING;
        }
        if (ui_element_type & DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND) {
            flags |= DSTUDIO_FLAG_USE_SLIDER_BACKGROUND_SETTING;
        }
        
        init_opengl_ui_elements(
            flags,
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
    (*shader_buffer) = dstudio_alloc(
        4096 * sizeof(GLchar),
        DSTUDIO_FAILURE_IS_FATAL
    );
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

void manage_cursor_position(int xpos, int ypos) {
    GLfloat motion;
    UIElements * ui_element;
    if (s_ui_element_index < 0) {
        return;
    }
    switch(g_ui_elements_array[s_ui_element_index].type) {
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
            motion = compute_knob_rotation(xpos, ypos);
            update_ui_element_motion(&g_ui_elements_array[s_ui_element_index], motion);
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            if (g_ui_elements_array[s_ui_element_index].enabled) {
                ui_element = &g_ui_elements_array[s_ui_element_index];
                motion = compute_slider_translation(ypos);
                
                compute_slider_in_motion_scissor_y(ui_element, motion);
                
                update_ui_element_motion(ui_element, motion);
                float slider_value = compute_slider_percentage_value(ypos);
                ui_element->application_callback_args = &slider_value;
                
                if (g_ui_elements_array[s_ui_element_index].application_callback) {
                    g_ui_elements_array[s_ui_element_index].application_callback(
                        &g_ui_elements_array[s_ui_element_index]
                    );
                }
            }
            break;
        default:
            break;
    }
}

void manage_mouse_button(int xpos, int ypos, int button, int action) {
    UIElements * ui_elements_p = 0;
    double timestamp = 0;
    GLfloat half_height;
    unsigned int clicked_none = 1;
    if (button == DSTUDIO_MOUSE_BUTTON_LEFT && action == DSTUDIO_MOUSE_BUTTON_PRESS) {
        clear_text_pointer();
        for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) {
            ui_elements_p = &g_ui_elements_array[i];
            if (
                xpos > ui_elements_p->areas.min_area_x &&
                xpos < ui_elements_p->areas.max_area_x &&
                ypos > ui_elements_p->areas.min_area_y &&
                ypos < ui_elements_p->areas.max_area_y &&
                ui_elements_p->enabled &&
                !(ui_elements_p->type & (DSTUDIO_ANY_BACKGROUND_TYPE | DSTUDIO_ANY_PATTERN_TYPE))
            ) {
                s_ui_element_index = i;
                clicked_none = 0;
                switch (ui_elements_p->type) {
                    case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
                        ui_elements_p->application_callback(ui_elements_p);
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
                        ui_elements_p->application_callback(ui_elements_p);
                        update_button(ui_elements_p);
                        break;
                    
                    case DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM:
                        timestamp = get_timestamp();
                        if (ui_elements_p->application_callback && timestamp - s_list_item_click_timestamp < DSTUDIO_DOUBLE_CLICK_DELAY) {
                            ui_elements_p->application_callback(ui_elements_p);
                        }
                        else {
                            select_item(ui_elements_p, DSTUDIO_SELECT_ITEM_WITH_CALLBACK);
                            s_list_item_click_timestamp = timestamp;
                            g_active_interactive_list = ui_elements_p->interactive_list;
                        }
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM:
                        timestamp = get_timestamp();
                        if (timestamp - s_list_item_click_timestamp < DSTUDIO_DOUBLE_CLICK_DELAY) {
                            update_text_pointer_context(ui_elements_p);
                        }
                        else {
                            select_item(ui_elements_p, DSTUDIO_SELECT_ITEM_WITH_CALLBACK);
                            s_list_item_click_timestamp = timestamp;
                        }
                        g_active_interactive_list = ui_elements_p->interactive_list;
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
                        s_x11_input_mask = g_x11_input_mask;
                        configure_input(0);
                        half_height = (ui_elements_p->coordinates_settings.scale_matrix[1].y * g_dstudio_viewport_height) / 2;
                        g_active_slider_range_min = ui_elements_p->areas.min_area_y + half_height;
                        g_active_slider_range_max = ui_elements_p->areas.max_area_y - half_height;
                        if (ui_elements_p->interactive_list) {
                            g_active_interactive_list = ui_elements_p->interactive_list;
                        }
                        __attribute__ ((fallthrough));
                                                                   
                    case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
                        g_ui_element_center_x = (int)(ui_elements_p->areas.min_area_x + ui_elements_p->areas.max_area_x) >> 1;
                        g_ui_element_center_y = (int)(ui_elements_p->areas.min_area_y + ui_elements_p->areas.max_area_y) >> 1;
                        break;
                        
                    default:
                        break;
                }
                break;
            }
        }
        if (clicked_none) {
            g_active_interactive_list = 0;
        }
    }
    else if(action == DSTUDIO_MOUSE_BUTTON_RELEASE) {
        s_ui_element_index = -1;
        if (s_x11_input_mask & PointerMotionMask) {
            configure_input(PointerMotionMask);
        }
    }
}


/* TODO: A better approach might be to pair updater and request together,
 * so consummer only call the updater if necessary.
*/
void register_ui_elements_updater(void (*updater)()) {
    s_updater_register[s_updater_register_index++].updater = updater;
}

inline void render_loop() {
    double framerate_limiter = 0;
    double framerate_limiter_timestamp = 0;
    WindowScale current_window_scale = {0};

    while (do_no_exit_loop()) {
        if (is_window_visible()) {
            framerate_limiter_timestamp = get_timestamp();
            listen_events();
    
            unsigned int render_all = need_to_redraw_all();
            render_all |= g_request_render_all;
            current_window_scale = get_window_scale();
            
            if (
                (current_window_scale.width != g_previous_window_scale.width) || 
                (current_window_scale.height != g_previous_window_scale.height)
            ) {
                render_all |= 1;
                update_viewport(current_window_scale);
                g_previous_window_scale.width = current_window_scale.width;
                g_previous_window_scale.height = current_window_scale.height;
                
            }
            
            update_ui_elements();
            
            // TODO: Investigate multiple unnessary render_viewport call
            if (render_viewport(render_all)) {
                swap_window_buffer();
            }
            g_request_render_all = 0;
            framerate_limiter = (g_framerate * 1000) - (get_timestamp() - framerate_limiter_timestamp) * 1000000;
        }
        else {
            framerate_limiter = DSTUDIO_WINDOW_IDLING_TIMEOUT;
            /* TODO: Rename or rewrite method because it may do more than just update ui elements.
            For instance it keep monitoring for instance creation and compute ressource usage. */
            update_ui_elements();
        }
        
        usleep((unsigned int) (framerate_limiter > 0 ? framerate_limiter : 0));
        //DSTUDIO_TRACE_ARGS("FPS: %lf FPS limiter: %u", 1/(get_timestamp() - framerate_limiter_timestamp), (unsigned int) (framerate_limiter > 0 ? framerate_limiter : 0))
    }
};

void render_ui_elements(UIElements * ui_elements) {
    switch(ui_elements->type) {
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_ROTATION);
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_SLIDE);
            break;

        case DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE:
        case DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND:
            glUniform4fv(g_ui_element_color_location, 1, &ui_elements->color.r);
            __attribute__ ((fallthrough));

        default:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_NONE);
            break;
    }
    glUniform1ui(g_no_texture_location, ui_elements->type & (DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND | DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE) ? 1 : 0);
    glBindTexture(GL_TEXTURE_2D, ui_elements->texture_ids[ui_elements->texture_index]);
        glBindVertexArray(ui_elements->vertex_array_object);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui_elements->index_buffer_object);
                glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid *) 0, ui_elements->count);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    ui_elements->render_state = DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED;
    
    /* After any slider movement, scissor must be reset accordingly to
     * the final motion value.
     */
    
    if (ui_elements->type == DSTUDIO_UI_ELEMENT_TYPE_SLIDER) {
        compute_slider_scissor_y(ui_elements);
    }
}


/* TODO: Implement efficient overlap mechanism. Use case:
 * What if text is rendered below slicer ?
 * Could add additionnal frame_buffer?
 */
unsigned int render_viewport(unsigned int render_all) {
    Vec4 tmp_tex_coordinates[4] = {0};
    Vec2 * menu_background_scale_matrix = 0;
    Vec4 * menu_background_vertex_attributes = 0;
    unsigned char pattern_width = 0;
    unsigned char pattern_height = 0;
    
    if (render_all) {
        glViewport(0, 0, g_previous_window_scale.width, g_previous_window_scale.height);
        glScissor(0, 0, g_previous_window_scale.width, g_previous_window_scale.height);
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);

        if (g_menu_background_enabled) {
            pattern_width = (unsigned char) g_menu_background_enabled->pattern_scale.width;
            pattern_height = (unsigned char) g_menu_background_enabled->pattern_scale.height;
            DSTUDIO_TRACE_ARGS("%d %d", g_menu_background_enabled->pattern_scale.width, g_menu_background_enabled->pattern_scale.height);

            menu_background_scale_matrix = g_menu_background_enabled->coordinates_settings.scale_matrix;
            menu_background_vertex_attributes = g_menu_background_enabled->vertex_attributes;
            
            memcpy(&tmp_tex_coordinates, menu_background_vertex_attributes, sizeof(GLfloat) * 16);

            GLfloat new_tex_coord_x = ((GLfloat) menu_background_scale_matrix[0].x * g_previous_window_scale.width) / pattern_width;
            GLfloat new_tex_coord_y = ((GLfloat) menu_background_scale_matrix[1].y * g_previous_window_scale.height) / pattern_height;
            
            GLfloat tex_offset_x = (GLfloat) (((g_previous_window_scale.width - g_dstudio_viewport_width) / 2) % pattern_width) / (GLfloat) pattern_width;
            GLfloat tex_offset_y = (GLfloat) (((g_previous_window_scale.height - g_dstudio_viewport_height) / 2) % pattern_height) / (GLfloat) pattern_height;
            
            
            menu_background_vertex_attributes[0].z += -tex_offset_x;
            menu_background_vertex_attributes[0].w += -tex_offset_y;
            
            menu_background_vertex_attributes[1].z += -tex_offset_x;
            menu_background_vertex_attributes[1].w = new_tex_coord_y -tex_offset_y;
            
            menu_background_vertex_attributes[2].z = new_tex_coord_x  -tex_offset_x;
            menu_background_vertex_attributes[2].w += -tex_offset_y;
            
            menu_background_vertex_attributes[3].z = new_tex_coord_x -tex_offset_x;
            menu_background_vertex_attributes[3].w = new_tex_coord_y -tex_offset_y;
            
            glBindBuffer(GL_ARRAY_BUFFER, g_menu_background_enabled->vertex_buffer_object);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, menu_background_vertex_attributes, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);    

            update_scale_matrix(g_menu_background_enabled->coordinates_settings.scale_matrix);
            render_ui_elements(g_menu_background_enabled);
            
            memcpy(menu_background_vertex_attributes, &tmp_tex_coordinates, sizeof(GLfloat) * 16);
            
            glBindBuffer(GL_ARRAY_BUFFER, g_menu_background_enabled->vertex_buffer_object);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, menu_background_vertex_attributes, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);  
        }
        
        glViewport(0, 0, g_dstudio_viewport_width, g_dstudio_viewport_height);
    }
    
    unsigned int background_rendering_start_index = render_all ? 0 : 1;
    unsigned int background_rendering_end_index;
    int layer_1_index_limit = -1;

    int scissor_offset_x = g_menu_background_enabled ? 0 : s_scissor_offset_x;
    int scissor_offset_y = g_menu_background_enabled ? 0 : s_scissor_offset_y;

    /* First of all, we get once every ui elements we want to render
     * in a single list, so we don't have to iterate many times 
     * through hundred of items and performs visibility or render
     * tests. */
     
     // We set the first requested element as the main background.
     s_ui_elements_requests_index = 0;
     s_ui_elements_requests[0] = &g_ui_elements_array[0];
     
     // Then we're gathering required ui_elements and defining layer 1 index limit
     for (unsigned int i = 1; i < g_dstudio_ui_element_count; i++) {
        if (g_ui_elements_array[i].render_state || (render_all && g_ui_elements_array[i].visible)) {
            s_ui_elements_requests[++s_ui_elements_requests_index] = &g_ui_elements_array[i];
            if (layer_1_index_limit < 0 && g_menu_background_enabled && s_ui_elements_requests[s_ui_elements_requests_index] >= g_menu_background_enabled) {
                layer_1_index_limit = s_ui_elements_requests_index;
            }
        }
     }
     if (s_ui_elements_requests_index == 0) {
        return 0;
     }
     if (layer_1_index_limit < 0) {
        layer_1_index_limit = s_ui_elements_requests_index+1;
     }

    /* Before rendering anything we're updating once buffer in GPU */
    // TODO: Could be done on the fly.
    for ( unsigned int i = 0; i <= s_ui_elements_requests_index; i++) {
        if (s_ui_elements_requests[i]->render_state == DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED) {
            update_gpu_buffer(s_ui_elements_requests[i]);
        }
    }

    /* To avoid unnecessary OpenGL calls and rendering process we're
     * Rendering once required ui elements in a framebuffer if submenu
     * is enabled. */
     
    if (g_menu_background_enabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffer_objects[0]);
    }
    else if (s_scissor_offset_x ||  s_scissor_offset_y) {
        glViewport(
            (g_previous_window_scale.width - g_dstudio_viewport_width)/2,
            (g_previous_window_scale.height - g_dstudio_viewport_height)/2, 
            g_dstudio_viewport_width, 
            g_dstudio_viewport_height
        );        
    }

    /* Render first layer background */
    background_rendering_end_index = render_all ? 1 : (unsigned int) layer_1_index_limit;
    for (unsigned int i = background_rendering_start_index; i < background_rendering_end_index; i++) {
        scissor_n_matrix_setting(i, 0, DSTUDIO_FLAG_NONE, scissor_offset_x, scissor_offset_y);
        render_ui_elements(s_ui_elements_requests[0]);
        if (background_rendering_start_index == 0) {
            break;
        }
    }

    /* Render first layer ui elements */
    for (unsigned int i = 1; i < (unsigned int) layer_1_index_limit; i++) {
        // Refresh  interactive list background and/or highligh1 when unselected 
        if (s_ui_elements_requests[i]->type == DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT && !(g_text_pointer_context.active && g_text_pointer_context.highlight == s_ui_elements_requests[i])) {
            scissor_n_matrix_setting(i, 0, DSTUDIO_FLAG_RESET_HIGHLIGHT_AREAS, 0, 0);
            render_ui_elements(s_ui_elements_requests[0]);
        }
        scissor_n_matrix_setting(i, i, DSTUDIO_FLAG_NONE, scissor_offset_x, scissor_offset_y);
        render_ui_elements(s_ui_elements_requests[i]);

    }

    if (g_menu_background_enabled) {

        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffer_objects[1]);
        /* Render second layer background */
        if (render_all) {
            glClearColor(0,0,0,0.0);
            glScissor(0,0, g_dstudio_viewport_width, g_dstudio_viewport_height);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        for (unsigned int i = layer_1_index_limit; i <= s_ui_elements_requests_index; i++) {
            if (is_background_ui_element(s_ui_elements_requests[i])) {
                scissor_n_matrix_setting(i, i, DSTUDIO_FLAG_NONE, 0, 0);
                render_ui_elements(s_ui_elements_requests[i]);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(
            (g_previous_window_scale.width - g_dstudio_viewport_width)/2,
            (g_previous_window_scale.height - g_dstudio_viewport_height)/2, 
            g_dstudio_viewport_width, 
            g_dstudio_viewport_height
        );

        /* Render all layers required areas as background */
        background_rendering_end_index = render_all ? 0 : s_ui_elements_requests_index;
        for (unsigned int i = background_rendering_start_index; i <= background_rendering_end_index; i++) {
            if (!(background_rendering_start_index == 0) && s_ui_elements_requests[i]->type == DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT ) {
                scissor_n_matrix_setting(i, -1, DSTUDIO_FLAG_RESET_HIGHLIGHT_AREAS, s_scissor_offset_x, s_scissor_offset_y);
                render_layers((DSTUDIO_FRAMEBUFFER_COUNT));
            }
            scissor_n_matrix_setting(i, -1, DSTUDIO_FLAG_NONE, s_scissor_offset_x, s_scissor_offset_y);
            render_layers(DSTUDIO_FRAMEBUFFER_COUNT);
            if (background_rendering_start_index == 0) {
                break;
            }
        }
        
        /* Render remaining ui elements */
        for (unsigned int i = layer_1_index_limit; i <= s_ui_elements_requests_index; i++) {
            if (!is_background_ui_element(s_ui_elements_requests[i])) {
                scissor_n_matrix_setting(i, i, DSTUDIO_FLAG_NONE, s_scissor_offset_x, s_scissor_offset_y);
                render_ui_elements(s_ui_elements_requests[i]);
            }
        }
    }
    glViewport(0, 0, g_dstudio_viewport_width, g_dstudio_viewport_height);
    return 1;
}

void set_prime_interface(unsigned int state) {
    UIInteractiveList * interactive_list;
    for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) {
        if (i < g_menu_background_index) {
            switch (g_ui_elements_array[i].type) {
                case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
                case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
                    if (g_ui_elements_array[i].interactive_list && state) {
                        interactive_list = g_ui_elements_array[i].interactive_list;
                        g_ui_elements_array[i].enabled = *interactive_list->source_data_count <= interactive_list->lines_number ? 0 : 1;
                        break;
                    }
                    __attribute__ ((fallthrough));
                case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
                case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
                case DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM:
                case DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM:
                    g_ui_elements_array[i].enabled = state;
                    
                default:
                    break;
            }
        }
    }
}

void set_ui_elements_visibility(UIElements * ui_elements, unsigned int state, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        ui_elements[i].render_state = state ? DSTUDIO_UI_ELEMENT_RENDER_REQUESTED : DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED;
        ui_elements[i].visible = state;
        
        switch(ui_elements[i].type) {
            case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
            case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
            case DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM:
                ui_elements[i].enabled = state;
                if (!(ui_elements[i].type & DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM)) 
                    ui_elements[i].texture_index = 0;
                break;
                
            default:
                break;
        }
    }
}

GLuint setup_texture_n_scale_matrix(
    unsigned int flags,
    GLuint texture_width,
    GLuint texture_height,
    const char * texture_filename,
    Vec2 * scale_matrix,
    PatternScale * pattern_scale
) {
    GLuint texture_id = 0;
    unsigned char * texture_data = 0;
    int alpha = flags & DSTUDIO_FLAG_USE_ALPHA;
    int enable_aa = flags & DSTUDIO_FLAG_USE_ANTI_ALIASING;
    int text_setting = flags & DSTUDIO_FLAG_USE_TEXT_SETTING;
    int texture_is_pattern = flags & DSTUDIO_FLAG_TEXTURE_IS_PATTERN;
    if (texture_is_pattern) {
        pattern_scale->width = texture_width; 
        pattern_scale->height = texture_height; 
    }
    get_png_pixel(texture_filename, &texture_data, alpha ? PNG_FORMAT_RGBA : PNG_FORMAT_RGB);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_is_pattern ? GL_REPEAT: GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_is_pattern ? GL_REPEAT: GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, texture_width, texture_height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enable_aa ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enable_aa ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST );
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    dstudio_free(texture_data);
    
    if (scale_matrix) {
        DEFINE_SCALE_MATRIX(scale_matrix, texture_width, texture_height)
        if (text_setting) {
            scale_matrix[0].x /= DSTUDIO_CHAR_SIZE_DIVISOR;
            scale_matrix[1].y /= DSTUDIO_CHAR_SIZE_DIVISOR;
        }
    }
    return texture_id;
}

void update_gpu_buffer(UIElements * ui_element_p) {
    switch (ui_element_p->type) {
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            glBindBuffer(GL_ARRAY_BUFFER, ui_element_p->instance_motions);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * ui_element_p->count, ui_element_p->instance_motions_buffer);
            break;

        case DSTUDIO_UI_ELEMENT_TYPE_TEXT:
        case DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM:
        case DSTUDIO_UI_ELEMENT_TYPE_TEXT_BACKGROUND:
        case DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM:
            glBindBuffer(GL_ARRAY_BUFFER, ui_element_p->instance_offsets);
                glBufferSubData(GL_ARRAY_BUFFER, 0, ui_element_p->text_buffer_size * sizeof(Vec4), ui_element_p->coordinates_settings.instance_offsets_buffer);
            break;
            
        case DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE:
            glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer_context.text_pointer->instance_offsets);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4), g_text_pointer_context.text_pointer->coordinates_settings.instance_offsets_buffer);
            
            glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer_context.text_pointer->instance_alphas);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat), g_text_pointer_context.text_pointer->instance_alphas_buffer);
            break;
            
        case DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT:
            glBindBuffer(GL_ARRAY_BUFFER, ui_element_p->instance_offsets);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * ui_element_p->count, ui_element_p->coordinates_settings.instance_offsets_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, ui_element_p->instance_alphas);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * ui_element_p->count, ui_element_p->instance_alphas_buffer);            
        
        default:
            break;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void update_ui_elements() {
    for (unsigned int i = 0; i < s_updater_register_index; i++) {
        s_updater_register[i].updater();
    }
}

void update_ui_element_motion(
    UIElements * ui_elements_p,
    float motion
) {
    *ui_elements_p->instance_motions_buffer = motion;
    ui_elements_p->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
}

void update_viewport(WindowScale window_scale) {
    (void) window_scale;
    GLfloat offset_x = 0.0;
    GLfloat offset_y = 0.0;

    if (window_scale.width > g_dstudio_viewport_width) {
        offset_x = ((GLfloat) (window_scale.width - g_dstudio_viewport_width));
    }
    
    if (window_scale.height > g_dstudio_viewport_height) {
        offset_y = ((GLfloat) (window_scale.height - g_dstudio_viewport_height));
    }
    
    s_scissor_offset_x = lroundf(offset_x / 2.0);
    s_scissor_offset_y = lroundf(offset_y / 2.0);
}
