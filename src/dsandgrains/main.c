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
#include "../instances.h"
#include "ui.h"
#include "instances.h"
#include "../system_usage.h"

int main(int argc, char ** argv) {
    SystemUsage system_usage = {0};

    Instances instances = {0};
    new_instance(INSTANCES_DIRECTORY, "dsandgrains");
    instances.contexts = malloc( sizeof(InstanceContext) );
    if (instances.contexts) {
        instances.count +=1;
    }
    
    UI ui = {0};
    system_usage.ui = &ui.system_usage;
    
    pthread_t ui_thread_id, system_usage_thread_id;
    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &ui_thread_id, NULL, ui_thread, &ui))
    DSTUDIO_RETURN_IF_FAILURE(pthread_create( &system_usage_thread_id, NULL, update_system_usage, &system_usage))
    
    DSTUDIO_RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))

    DSTUDIO_RETURN_IF_FAILURE(pthread_join(system_usage_thread_id, NULL))

    return 0;
}
