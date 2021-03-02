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

static jack_client_t * s_client;
static jack_status_t s_jack_status;

static void on_info_shutdown(jack_status_t code, const char *reason, void *arg) {
    (void) arg;
    (void) code;
    DSTUDIO_TRACE_ARGS("JACK server has shutdown: %s.", reason)
    // Should be called outside the function
    jack_client_close(s_client);
    //TODO: Start thread trying to restart client, waiting for server
}

static int process(jack_nframes_t nframes, void *arg) {
        (void) arg;
        (void) nframes;
        
        //~ jack_default_audio_sample_t * out;                      // type float. Correspond au format de l'échantillons
        //~ out = jack_port_get_buffer(output_port, nframes);       // on récupère l'addresse de la mémoire tampons pour les échantillons
                                                                //~ // out est un tableau de 1024 entrée.
 
        //~ memcpy(out, SOMETHING, 1024 * sizeof(jack_default_audio_sample_t));
        return 0;
}

DStudioAudioAPIError init_audio_api_client() {
        s_client = jack_client_open(g_application_name, JackNullOption, &s_jack_status, NULL);
    
        if (s_client == NULL) {
            DSTUDIO_TRACE_STR("jack_client_open() failed.")
            if (s_jack_status & JackServerFailed) {
                DSTUDIO_TRACE_STR("Unable to connect to JACK server.")
            }
            return DSTUDIO_AUDIO_API_CLIENT_CANNOT_CONNECT_TO_SERVER;
        }
                
        jack_set_process_callback(s_client, process, 0); 
        jack_on_info_shutdown(s_client, on_info_shutdown, 0); 	
        if(jack_activate(s_client)) {
            DSTUDIO_TRACE_STR("Cannot activate jack client.")
            return DSTUDIO_AUDIO_API_CANNOT_ENABLE_CLIENT;
        }
            
        return DSTUDIO_AUDIO_API_NO_ERROR;
}

DStudioAudioAPIError register_stereo_output_port(OutputPort * output_port, const char * left_port_name, const char * right_port_name) {
        output_port->left = jack_port_register(s_client, left_port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0); 
        output_port->right = jack_port_register(s_client, right_port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0); 

        if (output_port->left == NULL || output_port->right == NULL) {
            return DSTUDIO_AUDIO_API_OUTPUT_PORT_CANNOT_BE_CREATED;
        }
        return DSTUDIO_AUDIO_API_NO_ERROR;
}
