#include "dstudio_ui_engine.h"

int main(void)
{
    dstudio_init_memory_management();

    DStudioWindow window = dstudio_init_gui(1024, 768, "HELLO WORLD");
    
    DStudioBitmapWidget widget = dstudio_create_widget(DSTUDIO_KNOB_1_64x64_TEXTURE_PATH, NULL);

    render_bitmap_widget(widget);

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
