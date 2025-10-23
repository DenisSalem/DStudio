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

#include <string.h>
#include <png.h>
#include "dstudio_ui_engine.h"

/*static*/ DStudioGenericWidgetSharedObject generic_widget_shared_object = {
    {
        -1.0f, 1.0f,     0.0f, 0.0f,
        -1.0f, -1.0f,     0.0f, 1.0f,
        1.0f,  1.0f,     1.0f, 0.0f,
        1.0f, -1.0f,     1.0f, 1.0f
    },
    0, 
    0,
    0
};

//TODO is WIP. Should be static and managed by scene tree
void render_bitmap_widget(DStudioBitmapWidget widget) {
    glBindVertexArray(widget.vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, widget.vbo_id);
    
    glVertexAttribPointer(generic_widget_shared_object.pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glVertexAttribPointer(generic_widget_shared_object.tex_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(generic_widget_shared_object.pos_location);
    glEnableVertexAttribArray(generic_widget_shared_object.tex_location);
    
    glBindTexture(GL_TEXTURE_2D, widget.texture);
    glUseProgram(generic_widget_shared_object.shader_program_id);
} 
    
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

DStudioImage dstudio_create_texture(const char * filename) {
    DStudioImage image = dsudio_read_png(filename);
    GLuint texture_is_pattern = 0;  //TODO not implemented yet
    glGenTextures(1, &image.texture_id);
    glBindTexture(GL_TEXTURE_2D, image.texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_is_pattern ? GL_REPEAT: GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_is_pattern ? GL_REPEAT: GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, image.channels == 4 ? GL_RGBA : GL_RGB, image.width, image.height, 0, image.channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    dstudio_free(image.buffer);
    
    return image;
}

DStudioBitmapWidget dstudio_create_widget(const char * widget_filename, const char * background_filename) {
    DStudioBitmapWidget widget = {0};
    DStudioImage image = dstudio_create_texture(widget_filename);
    
    widget.height = image.height;
    widget.width = image.width;
    widget.texture = image.texture_id;
    
    if (background_filename != NULL) {
        image = dstudio_create_texture(background_filename);
        widget.background_height = image.height;
        widget.background_width = image.width;
        widget.background_texture = image.texture_id;
    }
    widget.vbo_id =  dstudio_create_gl_buffer(
        GL_ARRAY_BUFFER,
        generic_widget_shared_object.vertices,
        GL_STATIC_DRAW,
        sizeof(GLfloat) * 16
    );
    glGenVertexArrays(1, &widget.vao_id); 
    return widget;
}

DStudioBitmapWidget dstudio_create_knob(const char * widget_filename, const char * background_filename) {
    DStudioBitmapWidget widget = dstudio_create_widget(widget_filename, background_filename);
    widget.type = DSTUDIO_WIDGET_TYPE_KNOB;
    return widget;
}

static void dsudio_glfw_framebuffer_size_callback(GLFWwindow* window, int  g_dstudio_viewport_width, int  g_dstudio_viewport_height)
{
    (void) window;
    glViewport(0, 0,  g_dstudio_viewport_width,  g_dstudio_viewport_height);
}

DStudioWindow dstudio_init_gui(int width, int height, const char * title){
    GLFWwindow* window;

    dstudio_load_gl_extensions();


    if (!glfwInit()) {
        printf("DSTUDIO: glfwInit() == GLFW_FALSE\n");
        return NULL;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        int code = glfwGetError(NULL);
        printf("DSTUDIO: !window: %x\n", code);
        glfwTerminate();
        return NULL;
    }
    
    glfwSetFramebufferSizeCallback(window, dsudio_glfw_framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0,0.0,0.0, 0);

    generic_widget_shared_object.shader_program_id = dstudio_create_shader_program();
    generic_widget_shared_object.pos_location = glGetAttribLocation(generic_widget_shared_object.shader_program_id, "in_position");
    generic_widget_shared_object.tex_location = glGetAttribLocation(generic_widget_shared_object.shader_program_id, "in_TexCoord");
    return window;
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
        printf("Can't open asset \"%s\".\n", filename);
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
    image.channels  = png_get_channels(png_ptr, info_ptr);
    image.width     = png_get_image_width(png_ptr, info_ptr);
    image.height    = png_get_image_height(png_ptr, info_ptr);
    image.buffer    = dstudio_alloc(sizeof(uint8_t)*image.channels*image.width*image.height, DSTUDIO_FAILURE_IS_FATAL);
    
    for (uint_fast32_t h = 0; h < image.height; h++) {
        memcpy(&image.buffer[h*image.width*image.channels], row_pointers[h], image.width*image.channels);
    }
    
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return image;
}
