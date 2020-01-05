/*
 * Copyright 2019 Denis Salem
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

#include <pthread.h>
#include <semaphore.h>

#include "../common.h"
//#include "../fileutils.h"
//#include "../instances.h"
#include "../ressource_usage.h"
//#include "instances.h"
#include "ui.h"

const unsigned int g_dstudio_viewport_width = 800;
const unsigned int g_dstudio_viewport_height = 480;
const char g_application_name[] = "DSANDGRAINS";
const unsigned int g_dstudio_ui_element_count = sizeof(UIElementsStruct) / sizeof(UIElements);;

int main(int argc, char ** argv) {
    (void) argc;
    (void) argv;
    sem_init(&g_alloc_register_mutex, 0, 1);

    //DSTUDIO_EXIT_IF_FAILURE(set_physical_memory());

    //new_instance(DSANDGRAINS_INSTANCES_DIRECTORY, "dsandgrains");
    
    pthread_t ui_thread_id;
    pthread_t ressource_usage_thread_id;
    // pthread_t instances_thread_id;
    
    // TODO: Investigate thread priority.
    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, NULL))
    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ressource_usage_thread_id, NULL, update_ressource_usage, NULL))
    //DSTUDIO_RETURN_IF_FAILURE(pthread_create( &instances_thread_id, NULL, update_instances, NULL))

    //DSTUDIO_RETURN_IF_FAILURE(pthread_join(instances_thread_id, NULL))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ressource_usage_thread_id, NULL))
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    dstudio_free(0);
    return 0;
}
