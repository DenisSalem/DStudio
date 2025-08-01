#include "dstudio_ui_engine.h"

int main(void)
{
    dstudio_init_memory_management();
    dstudio_load_gl_extensions();
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(1024, 640, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("DEBUG WINDOW %lu\n", (unsigned long) window);
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glClearColor(0.5,0.5,0.5, 0);
    GLfloat vertices[] = {
        -1.0f,  1.0f, 0.0f,     0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
         1.0f,  1.0f, 0.0f,     1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,     1.0f, 1.0f
    };
    
    GLuint texture_id = dstudio_create_texture(DSTUDIO_FLAG_NONE, DSTUDIO_KNOB_1_64x64_TEXTURE_PATH);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    GLuint shader_program_id = dstudio_create_shader_program();
    GLuint vbo_id =  dstudio_create_gl_buffer(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW, sizeof(vertices));
    
    GLuint pos_attribute = glGetAttribLocation(shader_program_id, "in_position");
    GLuint tex_attribute = glGetAttribLocation(shader_program_id, "in_TexCoord");

    GLuint vao_id;
    glGenVertexArrays(1, &vao_id); 
    glBindVertexArray(vao_id);

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glVertexAttribPointer(pos_attribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(tex_attribute, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(pos_attribute);
    glEnableVertexAttribArray(tex_attribute);

    glUseProgram(shader_program_id);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
