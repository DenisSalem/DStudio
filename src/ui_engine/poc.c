#include "dstudio_ui_engine.h"

int main(void)
{
    dstudio_init_memory_management();
    dstudio_load_gl_extensions();
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1024, 640, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glClearColor(0.5,0.5,0.5, 0);
    GLfloat vertices[] = {
                           -0.7, -0.7, 0,
                           0.7, -0.7, 0,
                           0, 0.7, 0
    };
    
    
    GLuint shader_program_id = dstudio_create_shader_program();
    printf("SHADER PROGRAM ID: %d", shader_program_id);
    GLuint vbo_id =  dstudio_create_gl_buffer(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW, sizeof(vertices));
    // Get the 'pos' variable location inside this program
    GLuint pos_attribute_position = glGetAttribLocation(shader_program_id, "pos");

    GLuint vao_id;
    glGenVertexArrays(1, &vao_id); 
    glBindVertexArray(vao_id);

    // Bind it so that rest of vao operations affect this vao
    glBindVertexArray(vao_id);

    // buffer from which 'pos' will receive its data and the format of that data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glVertexAttribPointer(pos_attribute_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable this attribute array linked to 'pos'
    glEnableVertexAttribArray(pos_attribute_position);
    glUseProgram(shader_program_id);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
