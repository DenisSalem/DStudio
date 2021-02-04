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

#include <stdio.h> 
#include <stdlib.h>

#include <jack/jack.h>

#include "common.h"

jack_client_t * client;
jack_port_t * output_port;

void jack_shutdown(void *arg) {
    (void) arg;
}

int process(jack_nframes_t nframes, void *arg) {
        (void) arg;
        
        jack_default_audio_sample_t * out;                      // type float. Correspond au format de l'échantillons
        out = jack_port_get_buffer(output_port, nframes);       // on récupère l'addresse de la mémoire tampons pour les échantillons
                                                                // out est un tableau de 1024 entrée.
 
         (void) out;       
        // memcpy(out, SOMETHING, 1024 * sizeof(jack_default_audio_sample_t));
        return 0;
}

static int init_jack_client(
    jack_options_t options,
    jack_status_t * status,
    char *server_name)
{
        client = jack_client_open(g_application_name, options, status, server_name);
    
        if (client == NULL)
                return -1;
                
        jack_set_process_callback(client, process, 0); 
        jack_on_shutdown(client, jack_shutdown,0);

        output_port = jack_port_register(client, "Main Signal", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0); 
    
        if (output_port == NULL)
                return -2; 

        if (jack_activate(client))
                return -3; 

        return 0;
}

void * jack_client(void * arg) {
        (void) arg;
        
        jack_status_t status;
        char * server_name = NULL;
        int error = 0;
        
        error = init_jack_client(JackNullOption, &status, server_name);

        // TODO: Store error in arg to handle later
        if (error == -1){
                printf("jack_client_open() failed.\n");
                if (status & JackServerFailed) {
                        printf ("Unable to connect to JACK server.\n");
                }
        }
        else if (error == -2) {
                printf("Main output signal port cannot be create.\n");
        }
        else if (error == -3) {
                printf("Cannot activate jack client.\n");
        }
        
        //jack_client_close(client);
        
        return NULL;
}
