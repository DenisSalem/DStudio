#include <stdio.h>

#include "common.h"
#include "window_management.h"

int main(int argc, char ** argv) {
    (void) argc;
    (void) argv;
    dstudio_init_memory_management();
    init_window_context("DSANDGRAINS", 800, 640);
    
    
    
    window_render_loop();
    destroy_window_context();
    dstudio_free(0);
    return 0;
}
