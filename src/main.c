#include <stdio.h>
#include "extensions.h"
#ifdef DSTUDIO_USE_GLFW3
    #include <GLFW/glfw3.h>
#endif
#include <unistd.h>

#include "dsandgrains/ui.h"
#include "knobs.h"

int main() {
    if (!glfwInit()) {
        printf("!glfwIniti()\n");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT, "Knob PoC", NULL, NULL);
    
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
    // VERTEX SHADER
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, "../knob_vertex.shader");
    compile_shader(vertex_shader, &shader_buffer);
    
    free(shader_buffer);
    
    //FRAGMENT SHADER
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    load_shader(&shader_buffer, "../knob_fragment.shader");
    compile_shader(fragment_shader, &shader_buffer);

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

    Background background;
    init_background(&background);
    
    UiKnobs knobs;
    init_knobs(&knobs, 1, 64, "../assets/knob1_base.png", "../assets/knob1.png");


    
    //init_knob(&test_knobs, 0, 64.0, 320.0, 240.0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    while (!glfwWindowShouldClose(window)) {
        usleep(40000);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_id);
            render_background(&background, program_id);
            render_knobs(&knobs);
        glUseProgram(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program_id);
    free_knobs(&knobs);
    free_background(&background);
    glfwTerminate();
    return 0;
}
