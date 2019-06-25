#include <pthread.h>

#include "../common.h"
#include "ui.h"

int main(int argc, char ** argv) {
    UI ui = {0};
    UIKnobs * sample_knobs_p = &ui.sample_knobs;
    UIKnobs * sample_small_knobs_p = &ui.sample_small_knobs;
    UIKnobs * voice_knobs_p = &ui.voice_knobs;
    UISliders * sliders_dahdsr_p = &ui.sliders_dahdsr;
    UIArea * ui_areas = &ui.areas[0];
    UICallback * ui_callbacks = &ui.callbacks[0];
        
    init_knobs_cpu_side(sample_knobs_p, 8, 64, DSANDGRAINS_KNOB1_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_knobs_cpu_side(sample_small_knobs_p, 10, 48, DSANDGRAINS_KNOB2_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_knobs_cpu_side(voice_knobs_p, 3, 64, DSANDGRAINS_KNOB1_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    
    InitUIElementArray init_knobs_array[DSANDGRAINS_KNOBS_COUNT] = {
        {-0.8675, 0.25, 20.0,  85.0,  147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: START
        {-0.7075, 0.25, 84.0,  149.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: END
        {-0.5475, 0.25, 148.0, 213.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: GRAIN SIZE
        {-0.3875, 0.25, 212.0, 277.0, 147.0, 212.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: TRANSIENT THRESOLD
        {-0.8675, -.15, 20.0,  85.0,  243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LOOP DURATION
        {-0.7075, -.15, 84.0,  149.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: CLOUD SIZE
        {-0.5475, -.15, 148.0, 213.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: DISTRIBUTION BALANCE
        {-0.3875, -.15, 212.0, 277.0, 243.0, 308.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: SMOOTH TRANSITION
        
        {-0.222499,  0.254166, 286.0, 335.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION TUNE
        {-0.102500,  0.254166, 334.0, 383.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION PHASE
        {-0.222499,  0.008333, 286.0, 335.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO MODULATION DEPTH
        {-0.102500,  0.008333, 334.0, 383.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_2}, // SAMPLE: LFO MODULATION SIGNAL
        { 0.042499,  0.254166, 392.0, 441.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD TUNE
        { 0.162499,  0.254166, 440.0, 489.0, 154.0, 203.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD PHASE
        { 0.042499,  0.008333, 392.0, 441.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: LFO PITCH MOD DEPTH
        { 0.162499,  0.008333, 440.0, 489.0, 213.0, 261.0, DSTUDIO_KNOB_TYPE_2}, // SAMPLE: LFO PITCH MOD SIGNAL
        {-0.102500, -0.5375,   334.0, 383.0, 344.0, 393.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: AMOUNT
        { 0.162499, -0.808333, 440.0, 488.0, 409.0, 458.0, DSTUDIO_KNOB_TYPE_1}, // SAMPLE: PITCH
        { 0.5475,  0.270833, 586.0, 651.0, 142.0, 207.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : DENSITY
        { 0.3725, -0.129166, 516,   581.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : VOLUME
        { 0.5475, -0.129166, 586.0, 651.0, 238,   303.0, DSTUDIO_KNOB_TYPE_1},  // VOICE : PAN
    };

    InitUIElementArray * init_knob_array_p;
    
    for (int i = 0; i < DSANDGRAINS_SAMPLE_KNOBS; i++) {
        init_knob_array_p = &init_knobs_array[i];
        DSTUDIO_INIT_KNOB(sample_knobs_p, i, gl_x, gl_y, i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    for (int i = 0; i < DSANDGRAINS_SAMPLE_SMALL_KNOBS; i++) {
        init_knob_array_p = &init_knobs_array[8+i];
        DSTUDIO_INIT_KNOB(sample_small_knobs_p, i, gl_x, gl_y, 8+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    for (int i = 0; i < DSANDGRAINS_VOICE_KNOBS; i++) {
        init_knob_array_p = &init_knobs_array[18+i];
        DSTUDIO_INIT_KNOB(voice_knobs_p, i, gl_x, gl_y, 18+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    InitUIElementArray init_sliders_array[DSANDGRAINS_SLIDERS_COUNT] = {
        {-0.14, -0.816666, 338.0,  349.0, 414.0, 456.0, DSTUDIO_SLICER_TYPE_1},
        {-0.1, -0.816666, 20.0,  85.0, 414.0, 456.0, DSTUDIO_SLICER_TYPE_1},
        {-0.06, -0.816666, 20.0,  85.0, 414.0, 456.0, DSTUDIO_SLICER_TYPE_1},
        {-0.02, -0.816666, 20.0,  85.0, 414.0, 456.0, DSTUDIO_SLICER_TYPE_1},
        {0.02, -0.816666, 20.0,  85.0, 414.0, 456.0, DSTUDIO_SLICER_TYPE_1},
        {0.06, -0.816666, 20.0,  85.0, 414.0, 456.0, DSTUDIO_SLICER_TYPE_1}
    };
    
    InitUIElementArray * init_slider_array_p;
    for (int i = 0; i < DSANDGRAINS_SLIDERS_DAHDSR; i++) {
        init_slider_array_p = &init_sliders_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_p, i, gl_x, gl_y, 21+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    pthread_t ui_thread_id;

    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, &ui))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    return 0;
}
