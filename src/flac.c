/*
 * Copyright 2019, 2020 Denis Salem
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

#include <FLAC/stream_decoder.h>

#include "common.h"
#include "flac.h"

void (*s_client_error_callback)(const char * message) = 0;

unsigned call_count = 0;

FLAC__StreamDecoderWriteStatus write_callback(
    const FLAC__StreamDecoder *decoder,
    const FLAC__Frame *frame,
    const FLAC__int32 * const buffer[],
    void * client_data
) {
    (void) decoder;
    (void) frame;
    (void) buffer;
    (void) client_data;
    call_count+=frame->header.number.sample_number;
    
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
	(void)decoder;
    (void)client_data;

    FLAC__uint64 total_samples = 0;
    unsigned sample_rate = 0;
    unsigned channels = 0;
    unsigned bps = 0;

	/* print some stats */
	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
		/* save for later */
		total_samples = metadata->data.stream_info.total_samples;
		sample_rate = metadata->data.stream_info.sample_rate;
		channels = metadata->data.stream_info.channels;
		bps = metadata->data.stream_info.bits_per_sample;

		fprintf(stderr, "sample rate    : %u Hz\n", sample_rate);
		fprintf(stderr, "channels       : %u\n", channels);
		fprintf(stderr, "bits per sample: %u\n", bps);
        // TODO: fprintf(stderr, "total samples  : %" PRIu64 "\n", total_samples)
		fprintf(stderr, "total samples  : %lu\n", total_samples);
	}
}

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
	(void)decoder, (void)client_data;
    if (s_client_error_callback) {
        s_client_error_callback(FLAC__StreamDecoderErrorStatusString[status]);
    }
}


int load_flac(
    FILE * file,
    void (*client_error_callback)(const char * message),
    SharedSample * shared_sample
) {
    (void) file;
    s_client_error_callback = client_error_callback;
	FLAC__StreamDecoder *decoder = 0;
	FLAC__StreamDecoderInitStatus init_status;
    FLAC__bool decode_ok = 0;
    
    DSTUDIO_RETURN_IF_FAILURE((decoder = FLAC__stream_decoder_new()) == NULL)
	(void)FLAC__stream_decoder_set_md5_checking(decoder, true);

	init_status = FLAC__stream_decoder_init_FILE(decoder, file, write_callback, metadata_callback, error_callback, shared_sample);
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        client_error_callback(FLAC__StreamDecoderInitStatusString[init_status]);
        // TODO: GOES TO LOG
		fprintf(stderr, "ERROR: initializing decoder: %s\n", FLAC__StreamDecoderInitStatusString[init_status]);
        return 0;
    }
        
    // Test decorder
    decode_ok = FLAC__stream_decoder_process_until_end_of_stream(decoder);
    printf("call_count %u\n", call_count);
    return decode_ok;
}
