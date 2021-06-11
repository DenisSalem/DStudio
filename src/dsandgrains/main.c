/*
 * Copyright 2019, 2021 Denis Salem
 *
 * This file is part of DStudio.
 *
 * DStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DStudio. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * TODO :
 *      - Make type size explicit and not platform dependant.
 *      - Make the damn naming convention consistent.   
 */

#include <pthread.h>
#include <semaphore.h>

#include "../buttons.h"
#include "../common.h"
#include "../info_bar.h"
#include "../instances.h"
#include "../audio_api.h"
#include "../text_pointer.h"
#include "../voices.h"
#include "dsandgrains.h"
#include "instances.h"
#include "samples.h"
#include "ui.h"

const uint_fast32_t g_dstudio_viewport_width = 940;
const uint_fast32_t g_dstudio_viewport_height = 560;
const char   g_application_name[] = "DSANDGRAINS";

/* Allow generic DStudio UI features (like render loop) to deal with an 
 * array of UIElements. The size of this array is specific to the client
 * application and cannot be know in advance.
 */
const uint_fast32_t g_dstudio_ui_element_count = sizeof(UIElementsStruct) / sizeof(UIElements);;

int main(int argc, char ** argv) {
    (void) argc;
    (void) argv;
    
    dstudio_init_memory_management();
    
    setup_voice_sub_context(
        sizeof(Samples),
        bind_samples_interactive_list,
        set_samples_ui_context_from_parent_voice_list
    );

    dstudio_init_audio_api_client(dsandgrains_audio_process);

    // TODO: PASS CALLBACK TO INIT SOME AUDIO API, NSM or LADISH
    new_instance(DSANDGRAINS_INSTANCES_DIRECTORY, "dsandgrains");

    pthread_t ui_thread_id;

    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, NULL))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    dstudio_stop_audio_api_client();
    
    dstudio_free(0);
    return 0;
}
