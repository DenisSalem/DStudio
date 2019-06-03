#include <pthread.h>

#include "../common.h"
#include "ui.h"

int main(int argc, char ** argv) {
    UI ui = {0};
    UIKnobs * sample_knobs_p = &ui.sample_knobs;
    UIArea * ui_areas = &ui.areas[0];
    UICallback * ui_callbacks = &ui.callbacks[0];
    
    init_knobs_cpu_side(sample_knobs_p, 10, 64, "../assets/knob1.png");
    
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
    
    // Knobs SAMPLE : DENSITY
    init_knob(sample_knobs_p, 3, -0.3875, 0.0703);
    DSTUDIO_SET_AREA(3, 212.0, 277.0, 190.0, 255.0) 
    DSTUDIO_SET_UI_CALLBACK(3, update_knob, 3, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : TRANSIENT THRESOLD
    init_knob(sample_knobs_p, 4, -0.2275, 0.0703);
    DSTUDIO_SET_AREA(4, 276.0, 341.0, 190.0, 255.0) 
    DSTUDIO_SET_UI_CALLBACK(4, update_knob, 4, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : LOOP DURATION
    init_knob(sample_knobs_p, 5, -0.8675, -0.3291);
    DSTUDIO_SET_AREA(5, 20.0, 85.0, 286.0, 351.0) 
    DSTUDIO_SET_UI_CALLBACK(5, update_knob, 5, sample_knobs_p, DSTUDIO_KNOB_TYPE);
    
    // Knobs SAMPLE : CLOUD SIZE
    init_knob(sample_knobs_p, 6, -0.7075, -0.3291);
    DSTUDIO_SET_AREA(6, 84.0, 149.0, 286.0, 351.0) 
    DSTUDIO_SET_UI_CALLBACK(6, update_knob, 6, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : PITCH
    init_knob(sample_knobs_p, 7, -0.5475, -0.3291);
    DSTUDIO_SET_AREA(7, 148.0, 213.0, 286.0, 351.0) 
    DSTUDIO_SET_UI_CALLBACK(7, update_knob, 7, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : AMOUNT
    init_knob(sample_knobs_p, 8, -0.3875, -0.3291);
    DSTUDIO_SET_AREA(8, 212.0, 277.0, 286.0, 351.0) 
    DSTUDIO_SET_UI_CALLBACK(8, update_knob, 8, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    // Knobs SAMPLE : SMOOTH
    init_knob(sample_knobs_p, 9, -0.2275, -0.3291);
    DSTUDIO_SET_AREA(9, 276.0, 341.0, 286.0, 351.0) 
    DSTUDIO_SET_UI_CALLBACK(9, update_knob, 9, sample_knobs_p, DSTUDIO_KNOB_TYPE);

    pthread_t ui_thread_id;
    
    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, &ui))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    return 0;
}
