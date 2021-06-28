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

#include <pthread.h>
#include <unistd.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "info_bar.h"
static jack_client_t * s_client;
static jack_status_t s_jack_status;
static pthread_t s_restart_thread_id;

static void on_info_shutdown(jack_status_t code, const char *reason, void *arg) {
    (void) arg;
    (void) code;
    char message[128] = {0};
    sprintf(&message[0], "JACK server has shutdown: %s.", reason);
    dstudio_update_info_text(message);
    g_dstudio_audi_api_request_restart = 1;
}

static int process(jack_nframes_t nframes, void *arg) {
        (void) arg;
        // TODO : could be static
        int connection_left = 0;
        int connection_right = 0;
        VoiceContext * voice = 0;
        InstanceContext * instance = 0;
        jack_default_audio_sample_t * out_left = 0;
        jack_default_audio_sample_t * out_right = 0;
        ControllerValue * controller_value_p = NULL;
        ControllerValue * midi_ui_target_controller_value;
        UIElements * midi_ui_target = NULL;
        double value = 0;
        int match = 0;
        
        if (s_client_process == NULL || g_dstudio_audi_api_request_restart) {
            return 0;
        }
        
        switch(s_audio_api_request_state) {
            case DSTUDIO_AUDIO_API_REQUEST_NO_DATA_PROCESSING:
                s_audio_api_request_state = DSTUDIO_AUDIO_API_ACK_NO_DATA_PROCESSING;
                return 0;
                
            case DSTUDIO_AUDIO_API_ACK_NO_DATA_PROCESSING:
                return 0;
            
            case DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING:
                s_audio_api_request_state = DSTUDIO_AUDIO_API_ACK_DATA_PROCESSING;

            case DSTUDIO_AUDIO_API_ACK_DATA_PROCESSING:
            default:
                break;
        }
        
        for (uint_fast32_t instance_index = 0; instance_index < g_instances.count; instance_index++) {
            instance = g_instances.data + (instance_index*sizeof(VoiceContext)) ;
            for (uint_fast32_t voice_index = 0; voice_index < instance->voices.count; voice_index++) {
                voice = instance->voices.data + (voice_index*sizeof(VoiceContext));
                
                // Midi Block : get binded sliders and / or knobs event
                if (jack_port_connected(voice->ports.midi)) {
                    void * midi_buffer = jack_port_get_buffer(voice->ports.midi, nframes);
                    jack_midi_event_t in_event;
                    for (uint_fast32_t event_index = 0; event_index < jack_midi_get_event_count(midi_buffer); event_index++) {
                        jack_midi_event_get(&in_event, midi_buffer, event_index);
                        if ( (uint_fast8_t) 0xB0 <= in_event.buffer[0] && in_event.buffer[0] <= (uint_fast8_t) 0xBF) {
                            if (g_midi_capture_state == DSTUDIO_AUDIO_API_MIDI_CAPTURE_WAIT_FOR_INPUT && DSTUDIO_CURRENT_VOICE_CONTEXT == voice) {
                                voice->midi_binds[in_event.buffer[1]].ui_element = g_midi_ui_element_target;
                                voice->midi_binds[in_event.buffer[1]].controller_value = g_midi_ui_element_target->application_callback_args;
                                dstudio_update_info_text("Midi input has been binded!");
                                g_midi_capture_state = DSTUDIO_AUDIO_API_MIDI_CAPTURE_NONE;
                                g_midi_ui_element_target = NULL;
                            }
                            else if (voice->midi_binds[in_event.buffer[1]].ui_element) {
                                controller_value_p = voice->midi_binds[in_event.buffer[1]].controller_value;
                                midi_ui_target = voice->midi_binds[in_event.buffer[1]].ui_element;
                                midi_ui_target_controller_value = midi_ui_target->application_callback_args;
                                match =  midi_ui_target_controller_value ? controller_value_p->context_identifier == midi_ui_target_controller_value->context_identifier : 0;
                                                                                                
                                switch(midi_ui_target->type) {
                                    case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
                                        value = -KNOB_LOWEST_POSITION - (2.0 * KNOB_HIGHEST_POSITION) * ((double) in_event.buffer[2] / 127.0);
                                        if ( midi_ui_target_controller_value && match) {
                                            *midi_ui_target->instance_motions_buffer = value;
                                            update_knob_value_from_ui_element(midi_ui_target);
                                            midi_ui_target->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
                                        }
                                        else {
                                            DSTUDIO_TRACE;
                                            update_knob_value_from_motion(
                                                controller_value_p,
                                                value
                                            );
                                        }
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
                
                { // Audio Block 
                    connection_left = jack_port_connected(voice->ports.left);
                    connection_right = jack_port_connected(voice->ports.right);
                    
                    if (connection_left + connection_right) {
                        if (connection_left) {
                            out_left  = jack_port_get_buffer(voice->ports.left, nframes);
                            explicit_bzero(out_left, nframes*sizeof(float));
                        }
                        if (connection_right) {
                            out_right = jack_port_get_buffer(voice->ports.right, nframes);
                            explicit_bzero(out_right, nframes*sizeof(float));
                        }
                        s_client_process(
                            voice,
                            connection_left ? out_left : NULL,
                            connection_right ? out_right : NULL,
                            nframes
                        );
                    }
                }
            }
        }
        
        return 0;
}

static void * dstudio_thread_restart(void *args) {
    (void) args;
    DStudioAudioAPIError ret = dstudio_init_audio_api_client(s_client_process);
    if (ret != DSTUDIO_AUDIO_API_NO_ERROR) {
        return NULL;
    }
    VoiceContext * voice = 0;
    InstanceContext * instance = 0;
    for (uint_fast32_t instance_index = 0; instance_index < g_instances.count; instance_index++) {
        instance = g_instances.data + (instance_index * sizeof(InstanceContext));
        for (uint_fast32_t voice_index = 0; voice_index < instance->voices.count; voice_index++) {
            voice = instance->voices.data + (voice_index*sizeof(VoiceContext));
            dstudio_audio_api_high_level_port_registration(instance, voice);
        }
    }
    dstudio_update_info_text("Jack Server has been restarted.");
    return NULL;
}

DStudioAudioAPIError dstudio_audio_api_high_level_port_registration(InstanceContext * instance, VoiceContext * voice) {
    strcpy((char *) &s_audio_port_name_left_buffer, instance->name);
    strcpy((char *) &s_audio_port_name_right_buffer, instance->name);
    strcpy((char *) &s_midi_port_name_buffer, instance->name);
    strcat((char *) &s_audio_port_name_left_buffer, " > ");
    strcat((char *) &s_audio_port_name_right_buffer, " > ");
    strcat((char *) &s_midi_port_name_buffer, " > ");
    strcat((char *) &s_audio_port_name_left_buffer, voice->name);
    strcat((char *) &s_audio_port_name_right_buffer, voice->name);
    strcat((char *) &s_midi_port_name_buffer, voice->name);
    strcat((char *) &s_audio_port_name_left_buffer, " > L");
    strcat((char *) &s_audio_port_name_right_buffer, " >  R");
    
    DStudioAudioAPIError ret = dstudio_register_stereo_output_port(
        &DSTUDIO_CURRENT_VOICE_CONTEXT->ports,
        (const char *) &s_audio_port_name_left_buffer,
        (const char *) &s_audio_port_name_right_buffer
    );
    if (DSTUDIO_AUDIO_API_NO_ERROR != ret) {
        return ret;
    }
        
    return dstudio_register_midi_port(
        &DSTUDIO_CURRENT_VOICE_CONTEXT->ports,
        (const char *) &s_midi_port_name_buffer
    );
}

DStudioAudioAPIError dstudio_init_audio_api_client(void (*client_process_callback)(VoiceContext * voice, float * out_left, float * out_right, uint_fast32_t frame_size)) {
        s_client = jack_client_open((char*)g_dstudio_application_name, JackNoStartServer, &s_jack_status, NULL);
        s_client_process = client_process_callback;

        if(s_jack_status & JackFailure ){
            DSTUDIO_TRACE_STR("JACK: Overall operation failed.")
        }       
        if(s_jack_status & JackInvalidOption){
            DSTUDIO_TRACE_STR("JACK: The operation contained an invalid or unsupported option.")
        }
        if(s_jack_status & JackNameNotUnique){
            DSTUDIO_TRACE_STR("JACK: The desired client name is not unique.")
        }
        if(s_jack_status & JackServerStarted){
            DSTUDIO_TRACE_STR("JACK: Server is running.")
        }
        if (s_jack_status & JackServerFailed) {
            DSTUDIO_TRACE_STR("JACK: Unable to connect to JACK server.")
        }
        if(s_jack_status & JackServerError){
            DSTUDIO_TRACE_STR("JACK: Communication error with the JACK server.")
        }
        if(s_jack_status & JackNoSuchClient){
            DSTUDIO_TRACE_STR("JACK: Requested client does not exist.")
        }
        if(s_jack_status & JackLoadFailure){
            DSTUDIO_TRACE_STR("JACK: Unable to load internal client.")
        }
        if(s_jack_status & JackInitFailure){
            DSTUDIO_TRACE_STR("JACK: Unable to initialize client.")
        }
        if(s_jack_status & JackShmFailure ){
            DSTUDIO_TRACE_STR("JACK: Unable to access shared memory.")
        }
        if(s_jack_status & JackVersionError){
            DSTUDIO_TRACE_STR("JACK: Client's protocol version does not match.")
        }
        if(s_jack_status & JackBackendError){
            DSTUDIO_TRACE_STR("JACK: Backend Error.")
        }
        
        if (s_client == NULL) {
            g_dstudio_audi_api_request_restart = 1;
            return DSTUDIO_AUDIO_API_CLIENT_CANNOT_CONNECT_TO_SERVER;
        }
        
        jack_set_process_callback(s_client, process, 0);
        jack_on_info_shutdown(s_client, on_info_shutdown, 0); 	
        if(jack_activate(s_client)) {
            jack_client_close(s_client);
            s_client = 0;
            DSTUDIO_TRACE_STR("Cannot activate jack client.")
            g_dstudio_audi_api_request_restart = 1;
            return DSTUDIO_AUDIO_API_CANNOT_ENABLE_CLIENT;
        }
        s_audio_api_request_state = DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING;
        s_audio_api_client = s_client;
                
        return DSTUDIO_AUDIO_API_NO_ERROR;
}

void dstudio_audi_api_states_monitor() {
    if (g_dstudio_audi_api_request_restart) {
        sleep(1);
        g_dstudio_audi_api_request_restart = 0;
        pthread_create(&s_restart_thread_id, NULL, dstudio_thread_restart, NULL);
    }
}

uint_fast32_t dstudio_audio_api_voice_has_midi_input() {
    return jack_port_connected(DSTUDIO_CURRENT_VOICE_CONTEXT->ports.midi);
}


DStudioAudioAPIError dstudio_register_midi_port(AudioPort * output_port, const char * port_name) {
        if (!s_client) {
            return DSTUDIO_AUDIO_API_CLIENT_IS_NULL;
        }
        output_port->midi = jack_port_register(s_client, port_name, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0); 
        
        if (output_port->midi == NULL) {
            return DSTUDIO_AUDIO_API_MIDI_PORT_CANNOT_BE_CREATED;
        }
        
        return DSTUDIO_AUDIO_API_NO_ERROR;
}

DStudioAudioAPIError dstudio_register_stereo_output_port(AudioPort * output_port, const char * left_port_name, const char * right_port_name) {
        if (!s_client) {
            dstudio_update_info_text("Audio API client is null.");
            return DSTUDIO_AUDIO_API_CLIENT_IS_NULL;
        }
        output_port->left = jack_port_register(s_client, left_port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0); 
        output_port->right = jack_port_register(s_client, right_port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0); 
        
        if (output_port->left == NULL || output_port->right == NULL) {
            dstudio_update_info_text("Audio output port cannot be created.");
            return DSTUDIO_AUDIO_API_AUDIO_PORT_CANNOT_BE_CREATED;
        }
        
        return DSTUDIO_AUDIO_API_NO_ERROR;
}

DStudioAudioAPIError dstudio_rename_active_context_audio_port() {
    char audio_port_name_left_buffer[64] = {0};
    char audio_port_name_right_buffer[64] = {0};
    strcpy((char *) &audio_port_name_left_buffer, (char*) DSTUDIO_CURRENT_INSTANCE_CONTEXT->name);
    strcpy((char *) &audio_port_name_right_buffer, (char*) DSTUDIO_CURRENT_INSTANCE_CONTEXT->name);
    strcat((char *) &audio_port_name_left_buffer, " > ");
    strcat((char *) &audio_port_name_right_buffer, " > ");
    strcat((char *) &audio_port_name_left_buffer, (char*) DSTUDIO_CURRENT_VOICE_CONTEXT->name);
    strcat((char *) &audio_port_name_right_buffer, (char*) DSTUDIO_CURRENT_VOICE_CONTEXT->name);
    strcat((char *) &audio_port_name_left_buffer, " > L");
    strcat((char *) &audio_port_name_right_buffer, " > R");
    
    if (s_client && !g_dstudio_audi_api_request_restart) {
        if (
            jack_port_rename(s_client, DSTUDIO_CURRENT_VOICE_CONTEXT->ports.left, (const char *) &audio_port_name_left_buffer) ||
            jack_port_rename(s_client, DSTUDIO_CURRENT_VOICE_CONTEXT->ports.right, (const char *) &audio_port_name_right_buffer)
        ) {
            return DSTUDIO_AUDIO_API_AUDIO_PORT_RENAMING_FAILED;
        }
        return DSTUDIO_AUDIO_API_NO_ERROR;
    }
    return DSTUDIO_AUDIO_API_CLIENT_IS_NULL;
}

DStudioAudioAPIError dstudio_stop_audio_api_client() {
    if (s_client && !g_dstudio_audi_api_request_restart) {
        jack_client_close(s_client);
    }
    g_dstudio_audi_api_request_restart = 1;
    return DSTUDIO_AUDIO_API_NO_ERROR;
}

