#include "dstudio_ui_engine.h"

GLuint g_scale_matrix_id = 0;

static void update_scale_matrix(Vec2 * scale_matrix) {
    glUniformMatrix2fv(
        g_scale_matrix_id,
        1,
        GL_FALSE,
        (float *) scale_matrix
    );
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window;
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    // Re-render the scene because the current frame was drawn for the old resolution
}

int main(void)
{
    dstudio_init_memory_management();
    dstudio_load_gl_extensions();
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(1024, 640, "Hello World", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!window)
    {
        printf("DEBUG WINDOW %lu\n", (unsigned long) window);
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glClearColor(0.5,0.5,0.5, 0);
    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
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
    g_scale_matrix_id = glGetUniformLocation(shader_program_id, "scale_matrix");

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
