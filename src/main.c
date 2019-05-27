#include <stdio.h>
#include "extensions.h"
#ifdef DSTUDIO_USE_GLFW3
    #include <GLFW/glfw3.h>
#endif
#include <unistd.h>

#include "knobs.h"
#include "ui.h"

int main() {
    if (!glfwInit()) {
        printf("!glfwIniti()\n");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "Knob PoC", NULL, NULL);
    
    if (!window) {
            printf("!window\n");
            glfwTerminate();
            return 0;
    }
    
    glfwMakeContextCurrent(window);

    if(load_extensions() !=0) {
        printf("!load_extensions()\n");
        glfwTerminate();
        return 0;        
    }

    GLchar * shader_buffer = NULL;

    Knobs test_knobs;
    init_knobs(&test_knobs, 1, 64, 64, "../assets/knob1_base.png", "../assets/knob1.png");

    // VERTEX SHADER
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, "../knob_vertex.shader");
    compile_shader(vertex_shader, &shader_buffer);
    
    free(shader_buffer);
    
    //FRAGMENT SHADER
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    load_shader(&shader_buffer, "../knob_fragment.shader");
    compile_shader(fragment_shader, &shader_buffer);
    
    // IBO
    GLuint index_buffer_object = 0;
    glGenBuffers(1, &index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short int), test_knobs.vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // Texture
    glGenTextures(1, &test_knobs.texture_image_base_id);
    glBindTexture(GL_TEXTURE_2D, test_knobs.texture_image_base_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, test_knobs.texture_width, test_knobs.texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, test_knobs.image_base);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    init_knob(&test_knobs, 0, 64.0, 64.0, 320.0, 240.0);

    // VAO
    GLuint vertex_array_object;
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, test_knobs.items[0].vertex_buffer_object);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Linking Shader
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);

    glLinkProgram(program_id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    #ifdef DSTUDIO_DEBUG
    int info_log_length = 2048;
    char program_error_message[2048] = {0};

    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    glGetProgramInfoLog(program_id, info_log_length, NULL, program_error_message);
  
    if (strlen(program_error_message) != 0) {
        printf("%s\n", program_error_message);
    }
    #endif

    while (!glfwWindowShouldClose(window)) {
        usleep(40000);
        glEnable(GL_CULL_FACE);  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(program_id);
            glBindTexture(GL_TEXTURE_2D, test_knobs.texture_image_base_id);
                glBindVertexArray(vertex_array_object);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
                        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (GLvoid *) 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    free_knobs(&test_knobs);
    return 0;
}
