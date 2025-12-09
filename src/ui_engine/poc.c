#include "dstudio_ui_engine.h"

int main(void)
{
    dstudio_init_memory_management();

    dstudio_init_gui(1024, 768, "HELLO WORLD");

    DStudioSceneNode * scene_tree = dstudio_create_scene_tree();

    scene_tree->widget.bitmap = dstudio_create_knob(
        "../../../assets/knob_128x128.png",
        "../../../assets/knob_background_128x128.png"
    );
    scene_tree->widget.bitmap.width = 64;
    scene_tree->widget.bitmap.height = 64;

    while (!dstudio_window_should_close())
    {
        dstudio_event_manager();
        dstudio_render_scene_tree();
    }

    glfwTerminate();
    return 0;
}
