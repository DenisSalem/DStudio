#include "dstudio_ui_engine.h"

int main(void)
{
    dstudio_init_memory_management();

    DStudioWindow window = dstudio_init_gui(1024, 768, "HELLO WORLD");
    
    DStudioBitmapWidget widget = dstudio_create_widget(DSTUDIO_KNOB_1_64x64_TEXTURE_PATH, NULL);

    glBindVertexArray(widget.vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, widget.vbo_id);
    
    glVertexAttribPointer(generic_widget_shared_object.pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glVertexAttribPointer(generic_widget_shared_object.tex_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(generic_widget_shared_object.pos_location);
    glEnableVertexAttribArray(generic_widget_shared_object.tex_location);
    
    glBindTexture(GL_TEXTURE_2D, widget.texture);
    glUseProgram(generic_widget_shared_object.shader_program_id);

    while (!dstudio_window_should_close(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
