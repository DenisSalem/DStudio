#include <stdio.h>
#include "extensions.h"
#ifdef DSTUDIO_USE_GLFW3
    #include <GLFW/glfw3.h>
#endif

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

    // VERTEX SHADER
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader(&shader_buffer, "../knob_vertex.shader");
    compile_shader(vertex_shader, &shader_buffer);
    
    free(shader_buffer);
    
    //FRAGMENT SHADER
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    load_shader(&shader_buffer, "../knob_fragment.shader");
    compile_shader(fragment_shader, &shader_buffer);    

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
