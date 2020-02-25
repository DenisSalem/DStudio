/*
 * Copyright 2019, 2020 Denis Salem
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
#include "ui.h"

GLuint g_charset_8x18_texture_ids[2] = {0};
GLuint g_charset_4x9_texture_ids[2] = {0};
Vec2 g_charset_8x18_scale_matrix[2] = {0};
Vec2 g_charset_4x9_scale_matrix[2] = {0};
GLuint g_shader_program_id = 0;
GLuint g_scale_matrix_id = 0;
GLuint g_motion_type_location = 0;
GLuint g_no_texture_location = 0;
GLuint g_ui_element_color_location = 0;

unsigned int g_framerate = DSTUDIO_FRAMERATE;
UIElements * g_menu_background_enabled = 0;
unsigned int g_menu_background_index = 0;
unsigned int g_request_render_all = 0;

static int                  s_active_slider_range_max = 0;
static int                  s_active_slider_range_min = 0;
static double               s_list_item_click_timestamp = 0;
static int                  s_ui_element_center_x = 0;
static int                  s_ui_element_center_y = 0;
static int                  s_ui_element_index = -1;
static UpdaterRegister *    s_updater_register = 0;
static unsigned int         s_updater_register_index = 0;

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

void set_prime_interface(unsigned int state) {
    for (unsigned int i = 0; i < g_dstudio_ui_element_count; i++) {
        if (i < g_menu_background_index) {
            switch (g_ui_elements_array[i].type) {
                case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
                case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
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
    else if (slider_background_setting) {
        vertex_attributes[1].w /= (GLfloat) 3.0;
        vertex_attributes[3].w /= (GLfloat) 3.0;
    }
    else if (texture_is_pattern) {
        vertex_attributes[1].w *= ((GLfloat) ui_elements->scale_matrix[1].y * g_dstudio_viewport_height) / DSTUDIO_PATTERN_SCALE;
        vertex_attributes[2].z *= ((GLfloat) ui_elements->scale_matrix[0].x * g_dstudio_viewport_width) / DSTUDIO_PATTERN_SCALE;
        vertex_attributes[3].z *= ((GLfloat) ui_elements->scale_matrix[0].x * g_dstudio_viewport_width) / DSTUDIO_PATTERN_SCALE;
        vertex_attributes[3].w *= ((GLfloat) ui_elements->scale_matrix[1].y * g_dstudio_viewport_height) / DSTUDIO_PATTERN_SCALE;
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

void init_text() {
    g_charset_8x18_texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_TEXT_SETTING,
        DSTUDIO_CHAR_TABLE_8X18_WIDTH,
        DSTUDIO_CHAR_TABLE_8X18_HEIGHT,
        DSTUDIO_CHAR_TABLE_8X18_ASSET_PATH,
        g_charset_8x18_scale_matrix
    );
    
    g_charset_4x9_texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_TEXT_SETTING,
        DSTUDIO_CHAR_TABLE_4X9_WIDTH,
        DSTUDIO_CHAR_TABLE_4X9_HEIGHT,
        DSTUDIO_CHAR_TABLE_4X9_ASSET_PATH,
        g_charset_4x9_scale_matrix
    );
}

void init_threaded_ui_element_updater_register(unsigned int updater_count) {
    s_updater_register = dstudio_alloc(updater_count * sizeof(UpdaterRegister));
}

void init_ui() {    
    init_context(
        g_application_name,
        g_dstudio_viewport_width,
        g_dstudio_viewport_height
    );
    set_mouse_button_callback(manage_mouse_button);
    set_cursor_position_callback(manage_cursor_position);
    	
    DSTUDIO_EXIT_IF_FAILURE(load_extensions())
    
    create_shader_program(&g_shader_program_id);
    
    g_motion_type_location = glGetUniformLocation(g_shader_program_id, "motion_type");
    g_no_texture_location = glGetUniformLocation(g_shader_program_id, "no_texture");
    g_ui_element_color_location = glGetUniformLocation(g_shader_program_id, "ui_element_color");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glUseProgram(g_shader_program_id);
};

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
    if (ui_element_type & (DSTUDIO_UI_ELEMENT_TYPE_TEXT | DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM)) {
        min_area_x = (1 + gl_x - scale_matrix[0].x) * (g_dstudio_viewport_width >> 1); 
    }
    else {
        min_area_x = (1 + gl_x) * (g_dstudio_viewport_width >> 1) - (area_width / 2); 
    }
    if (ui_element_type & DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND) {
        min_area_y = (1 - gl_y - scale_matrix[1].y*2) * (g_dstudio_viewport_height >> 1);
        printf("min_area_y: %lf, height: %lf\n", min_area_y, area_height);
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
        
        ui_elements_array[i].instance_alphas_buffer = dstudio_alloc(sizeof(GLfloat) * instances_count);        
        ui_elements_array[i].instance_motions_buffer = dstudio_alloc(sizeof(GLfloat) * instances_count);
        ui_elements_array[i].instance_offsets_buffer = dstudio_alloc(sizeof(Vec4) * instances_count);
        
        for (unsigned int j = 0; j < instances_count; j++) {
            ui_elements_array[i].instance_alphas_buffer[j] = 1.0;
            if (ui_element_type == DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND) {
                ui_elements_array[i].instance_offsets_buffer[j].x = gl_x + (x * offset_x);
                ui_elements_array[i].instance_offsets_buffer[j].y = gl_y - (j * scale_matrix[1].y * 2) + (y * offset_y);
                if (j == instances_count-1) {
                    ui_elements_array[i].instance_offsets_buffer[j].w = 2.0/3.0;
                }
                else if (j != 0) {
                    ui_elements_array[i].instance_offsets_buffer[j].w = 1.0/3.0;
                }
            }
            else {
                ui_elements_array[i].instance_offsets_buffer[j].x = gl_x + (j * scale_matrix[0].x * 2) + (x * offset_x);
                ui_elements_array[i].instance_offsets_buffer[j].y = gl_y + (y * offset_y); 
            }
        }
        
        ui_elements_array[i].scissor.x = min_area_x + computed_area_offset_x;
        ui_elements_array[i].scissor.y = g_dstudio_viewport_height - max_area_y - computed_area_offset_y;
        ui_elements_array[i].scissor.width = max_area_x - min_area_x;
        ui_elements_array[i].scissor.height = max_area_y - min_area_y;
        
        ui_elements_array[i].count = instances_count;
        
        if(texture_ids) {
            memcpy(ui_elements_array[i].texture_ids, texture_ids, sizeof(GLuint) * 2);
        }
        ui_elements_array[i].scale_matrix = scale_matrix;
        
        ui_elements_array[i].visible = (flags & DSTUDIO_FLAG_IS_VISIBLE) != 0;
        ui_elements_array[i].request_render = ui_elements_array[i].visible;
        
        
        if (ui_element_type & (DSTUDIO_UI_ELEMENT_TYPE_TEXT | DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM)) {
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
    double timestamp = 0;
    GLfloat half_height;
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
                ui_elements_p->type != DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND && ui_elements_p->type != DSTUDIO_UI_ELEMENT_TYPE_PATTERN
            ) {
                s_ui_element_index = i;
                switch (ui_elements_p->type) {
                    case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
                        ui_elements_p->application_callback(ui_elements_p);
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
                        ui_elements_p->application_callback(ui_elements_p);
                        update_button(ui_elements_p);
                        break;
                    
                    case DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM:
                        timestamp = get_timestamp();
                        if (ui_elements_p->interactive_list->editable && timestamp - s_list_item_click_timestamp < DSTUDIO_DOUBLE_CLICK_DELAY) {
                            update_text_pointer_context(ui_elements_p);
                        }
                        else {
                            select_item(ui_elements_p, DSTUDIO_SELECT_ITEM_WITH_CALLBACK);
                            s_list_item_click_timestamp = timestamp;
                        }
                        break;
                        
                    case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
                        half_height = (ui_elements_p->scale_matrix[1].y * g_dstudio_viewport_height) / 2;
                        s_active_slider_range_min = ui_elements_p->areas.min_area_y + half_height;
                        s_active_slider_range_max = ui_elements_p->areas.max_area_y - half_height;
                        __attribute__ ((fallthrough));
                                                                   
                    case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
                        s_ui_element_center_x = (int)(ui_elements_p->areas.min_area_x + ui_elements_p->areas.max_area_x) >> 1;
                        s_ui_element_center_y = (int)(ui_elements_p->areas.min_area_y + ui_elements_p->areas.max_area_y) >> 1;
                        break;
                        
                    default:
                        break;
                }
                break;
            }
        }
    }
    else if(action == DSTUDIO_MOUSE_BUTTON_RELEASE) {
        s_ui_element_index = -1;
    }
}

void register_threaded_ui_elements_updater(ThreadControl * thread_control, void (*updater)()) {
    s_updater_register[s_updater_register_index].thread_control = thread_control;
    s_updater_register[s_updater_register_index++].updater = updater;
}

inline void render_loop() {
    while (do_no_exit_loop()) {
        usleep(g_framerate);
        update_threaded_ui_element();
        unsigned int render_all = need_to_redraw_all();
        render_all |= g_request_render_all;
        g_request_render_all = 0;
        if (g_menu_background_enabled) {
            render_all |= g_menu_background_enabled->request_render;
        }
        render_viewport(render_all);
        swap_window_buffer();
        listen_events();
    }
};

static void render_sub_menu_region() {
    for (unsigned int i = g_menu_background_index; i < g_dstudio_ui_element_count; i++) {
        if (g_ui_elements_array[i].visible) {
            glUniformMatrix2fv(
                g_scale_matrix_id,
                1,
                GL_FALSE,
                (float *) g_ui_elements_array[i].scale_matrix
            );
            render_ui_elements(&g_ui_elements_array[i]);
        }
    }
}

void render_ui_elements(UIElements * ui_elements) {
    switch(ui_elements->type) {
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_ROTATION);
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_SLIDE);
            break;

        case DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE:
            glUniform4fv(g_ui_element_color_location, 1, &ui_elements->color.r);
            __attribute__ ((fallthrough));

        default:
            glUniform1ui(g_motion_type_location, DSTUDIO_MOTION_TYPE_NONE);
            break;
    }
    glUniform1ui(g_no_texture_location, ui_elements->type == DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE ? 1 : 0);

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
    if (g_ui_elements_array[0].request_render || render_all) {       
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
        g_ui_elements_array[0].request_render = 0;
    }
    else {
        for (unsigned int i = 1; i < g_dstudio_ui_element_count; i++) {
            if (g_ui_elements_array[i].request_render) {
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
        if (g_ui_elements_array[i].request_render || (render_all && g_ui_elements_array[i].visible)) {
            glScissor(
                g_ui_elements_array[i].scissor.x,
                g_ui_elements_array[i].scissor.y,
                g_ui_elements_array[i].scissor.width,
                g_ui_elements_array[i].scissor.height
            );
            
            if (g_menu_background_enabled && g_ui_elements_array[i].request_render && !render_all && i > g_menu_background_index && !g_menu_background_enabled->request_render) {
               render_sub_menu_region();
            }
            
            glUniformMatrix2fv(
                g_scale_matrix_id,
                1,
                GL_FALSE,
                (float *) g_ui_elements_array[i].scale_matrix
            );
            
            render_ui_elements(&g_ui_elements_array[i]);
            g_ui_elements_array[i].request_render = 0;
                                    
            if (g_menu_background_enabled && i < g_menu_background_index && !g_menu_background_enabled->request_render) {
                render_sub_menu_region();
            }
        }
    }
}

void set_ui_elements_visibility(UIElements * ui_elements, unsigned int state, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        ui_elements[i].request_render = state;
        ui_elements[i].visible = state;
        switch(ui_elements[i].type) {
            case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
            case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
                ui_elements[i].enabled = state;
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
    Vec2 * scale_matrix
) {
    GLuint texture_id = 0;
    unsigned char * texture_data = 0;
    int alpha = flags & DSTUDIO_FLAG_USE_ALPHA;
    int enable_aa = flags & DSTUDIO_FLAG_USE_ANTI_ALIASING;
    int text_setting = flags & DSTUDIO_FLAG_USE_TEXT_SETTING;
    int texture_is_pattern = flags & DSTUDIO_FLAG_TEXTURE_IS_PATTERN;
    
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

void update_threaded_ui_element() {
    sem_t * mutex = 0;
    ThreadControl * thread_control = 0;
    for (unsigned int i = 0; i < s_updater_register_index; i++) {
        thread_control = s_updater_register[i].thread_control;
        mutex = thread_control->shared_mutex ? thread_control->shared_mutex : &thread_control->mutex;
        sem_wait(mutex);
        if (!thread_control->update) {
            sem_post(mutex);
            continue;
        }
        s_updater_register[i].updater();
        thread_control->update = 0;
        sem_post(mutex);
    }
}

void update_ui_element_motion(
    UIElements * ui_elements_p,
    float motion
) {
    *ui_elements_p->instance_motions_buffer = motion;
    glBindBuffer(GL_ARRAY_BUFFER, ui_elements_p->instance_motions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * ui_elements_p->count, ui_elements_p->instance_motions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ui_elements_p->request_render = 1;
}
