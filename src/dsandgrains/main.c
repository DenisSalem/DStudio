#include <pthread.h>

#include "../common.h"
#include "../instances.h"
#include "ui.h"
#include "instances.h"


int main(int argc, char ** argv) {
    
    Instances instances = {0};
    new_instance(INSTANCES_DIRECTORY, "dsandgrains");
    instances.contexts = malloc( sizeof(InstanceContext) );
    
    UI ui = {0};
    UIKnobs * sample_knobs_p = &ui.sample_knobs;
    UIKnobs * sample_small_knobs_p = &ui.sample_small_knobs;
    UIKnobs * voice_knobs_p = &ui.voice_knobs;
    UISliders * sliders_dahdsr_p = &ui.sliders_dahdsr;
    UISliders * sliders_dahdsr_pitch_p = &ui.sliders_dahdsr_pitch;
    UISliders * sliders_dahdsr_lfo_p = &ui.sliders_dahdsr_lfo;
    UISliders * sliders_dahdsr_lfo_pitch_p = &ui.sliders_dahdsr_lfo_pitch;
    UISliders * sliders_equalizer_p = &ui.sliders_equalizer;
    UIArea * ui_areas = &ui.areas[0];
    UICallback * ui_callbacks = &ui.callbacks[0];
        
    init_knobs_cpu_side(sample_knobs_p, 8, 64, DSANDGRAINS_KNOB1_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_knobs_cpu_side(sample_small_knobs_p, 10, 48, DSANDGRAINS_KNOB2_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_knobs_cpu_side(voice_knobs_p, 3, 64, DSANDGRAINS_KNOB1_ASSET_PATH, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_pitch_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_lfo_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_dahdsr_lfo_pitch_p, 6, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);
    init_sliders_cpu_side(sliders_equalizer_p, 8, 10, DSANDGRAINS_SLIDER1_ASSET_PATH,  DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT);

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

    InitUIElementArray * init_slider_array_p;

    InitUIElementArray init_sliders_dahdsr_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {-0.14, -0.816666, 338.0, 349.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1,  -0.816666, 354.0, 365.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.06, -0.816666, 370.0, 381.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.02, -0.816666, 386.0, 397.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {0.02, -0.816666,  402.0, 413.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1},
        {0.06, -0.816666,  418.0, 429.0, 413.0, 457.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_p, i, gl_x, gl_y, 21+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    
    InitUIElementArray init_sliders_dahdsr_pitch_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {0.0025, -0.558333, 395.0, 406.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0425, -0.558333, 411.0, 422.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0825, -0.558333, 427.0, 438.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1225, -0.558333, 443.0, 454.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1625, -0.558333, 459.0, 470.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1},
        {0.2025, -0.558333, 475.0, 486.0, 352.0, 396.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_pitch_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_pitch_p, i, gl_x, gl_y, 27+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray init_sliders_dahdsr_lfo_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {-0.2625, -0.283333, 289.0, 300.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.2225, -0.283333, 305.0, 316.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1825, -0.283333, 321.0, 332.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1425, -0.283333, 337.0, 348.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.1025, -0.283333, 353.0, 364.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {-0.0625, -0.283333, 369.0, 380.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_lfo_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_lfo_p, i, gl_x, gl_y, 33+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray init_sliders_dahdsr_lfo_pitch_array[DSANDGRAINS_SLIDERS_COUNT_PER_GROUP] = {
        {0.0025, -0.283333, 395.0, 406.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0425, -0.283333, 411.0, 422.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.0825, -0.283333, 427.0, 438.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1225, -0.283333, 443.0, 454.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.1625, -0.283333, 459.0, 470.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1},
        {0.2025, -0.283333, 475.0, 486.0, 285.0, 329.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_SLIDERS_COUNT_PER_GROUP; i++) {
        init_slider_array_p = &init_sliders_dahdsr_lfo_pitch_array[i];
        DSTUDIO_INIT_SLIDER(sliders_dahdsr_lfo_pitch_p, i, gl_x, gl_y, 39+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }

    InitUIElementArray init_sliders_equalizer_array[DSANDGRAINS_EQUALIZER_SLIDERS_COUNT] = {
        {0.32, -0.475, 522.0, 533.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.36, -0.475, 538.0, 549.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.4,  -0.475, 554.0, 565.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.44, -0.475, 570.0, 581.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.48, -0.475, 586.0, 597.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.52, -0.475, 602.0, 613.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.56, -0.475, 618.0, 629.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1},
        {0.60, -0.475, 634.0, 645.0, 331.0, 375.0, DSTUDIO_SLIDER_TYPE_1}
    };
    
    for (int i = 0; i < DSANDGRAINS_EQUALIZER_SLIDERS_COUNT; i++) {
        init_slider_array_p = &init_sliders_equalizer_array[i];
        DSTUDIO_INIT_SLIDER(sliders_equalizer_p, i, gl_x, gl_y, 45+i, min_area_x, max_area_x, min_area_y, max_area_y, ui_element_type)
    }
    pthread_t ui_thread_id;

    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, &ui))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    return 0;
}
