#include <pthread.h>

#include "../common.h"
#include "ui.h"

int main(int argc, char ** argv) {
    UI ui = {0};
    UIKnobs * sample_knobs_p = &ui.sample_knobs;
    UIArea * ui_areas = &ui.areas[0];
    UICallback * ui_callbacks = &ui.callbacks[0];
    
    init_knobs_cpu_side(sample_knobs_p, 8, 64, "../assets/knob1.png");
    init_knob(sample_knobs_p, 4, -0.8675, -0.3291);
    init_knob(sample_knobs_p, 5, -0.7075, -0.3291);
    init_knob(sample_knobs_p, 6, -0.5475, -0.3291);
    init_knob(sample_knobs_p, 7, -0.3875, -0.3291);
    
    // Knobs SAMPLE : START
    init_knob(sample_knobs_p, 0, -0.8675, 0.0703);
    DSTUDIO_SET_AREA(0, 20.0, 85.0, 190.0, 255.0) 
    DSTUDIO_SET_UI_CALLBACK(0, update_knob, 0, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : END
    init_knob(sample_knobs_p, 1, -0.7075, 0.0703);
    DSTUDIO_SET_AREA(1, 84.0, 149.0, 190.0, 255.0) 
    DSTUDIO_SET_UI_CALLBACK(1, update_knob, 1, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : GRAIN SIZE
    init_knob(sample_knobs_p, 2, -0.5475, 0.0703);
    DSTUDIO_SET_AREA(2, 148.0, 213.0, 190.0, 255.0) 
    DSTUDIO_SET_UI_CALLBACK(2, update_knob, 2, sample_knobs_p, DSTUDIO_KNOB_TYPE);
    
    // Knobs SAMPLE : Density
    init_knob(sample_knobs_p, 3, -0.3875, 0.0703);
    DSTUDIO_SET_AREA(3, 212.0, 277.0, 190.0, 255.0) 
    DSTUDIO_SET_UI_CALLBACK(3, update_knob, 3, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    pthread_t ui_thread_id;
    
    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, &ui))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    return 0;
}
