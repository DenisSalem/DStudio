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
    
    GLFWwindow* window = glfwCreateWindow(DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT, "DSANDGRAINS", NULL, NULL);
    
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

    GLuint interactive_program_id, non_interactive_program_id;
    create_shader_program(&interactive_program_id, &non_interactive_program_id);

    Background background;
    init_background(&background);
    
    UiKnobs sample_knobs;
    init_knobs(&sample_knobs, 8, 64, "../assets/knob1.png");

    init_knob(&sample_knobs, 0, -0.8675, 0.0703);
    init_knob(&sample_knobs, 1, -0.7075, 0.0703);
    init_knob(&sample_knobs, 2, -0.5475, 0.0703);
    init_knob(&sample_knobs, 3, -0.3875, 0.0703);
    init_knob(&sample_knobs, 4, -0.8675, -0.3291);
    init_knob(&sample_knobs, 5, -0.7075, -0.3291);
    init_knob(&sample_knobs, 6, -0.5475, -0.3291);
    init_knob(&sample_knobs, 7, -0.3875, -0.3291);
    finalize_knobs(&sample_knobs);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    while (!glfwWindowShouldClose(window)) {
        usleep(40000);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(non_interactive_program_id);
            render_background(&background);
            
        glUseProgram(interactive_program_id);
            render_knobs(&sample_knobs);
            
        glUseProgram(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(non_interactive_program_id);
    glDeleteProgram(interactive_program_id);
    free_knobs(&sample_knobs);
    free_background(&background);
    glfwTerminate();
    return 0;
}
