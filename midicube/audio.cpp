/*
 * audio.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */
#include "audio.h"
#include <jack/jack.h>
#include <math.h>
#include <iostream>

#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

int g_sample_rate_callback(jack_nframes_t nframes, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;
	handler->sample_rate_callback(nframes);
	return 0;
}

int g_process(jack_nframes_t nframes, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;
	return handler->process(nframes);
}

void AudioHandler::init() {
	jack_options_t options = JackNullOption;
	jack_status_t status;

	//Open client
	client = jack_client_open("MIDICube", options, &status, NULL);
	if (client == NULL) {
		throw AudioException("Couldn't connect to JACK Server!");
	}
	//Set callback
	jack_set_sample_rate_callback(client, g_sample_rate_callback, this);
	jack_set_process_callback(client, g_process, this);
	//Create output ports
	output_port_1 = jack_port_register(client, "stereo_output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	if (output_port_1 == NULL) {
		throw AudioException("Couldn't create output port!");
	}
	output_port_2 = jack_port_register(client, "stereo_output_2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	if (output_port_2 == NULL) {
		throw AudioException("Couldn't create output port!");
	}
	//Start callback
	if (jack_activate(client)) {
		throw AudioException("Couldn't activate JACK client!");
	}
	//Connect output port
	const char** ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
	if (ports == NULL) {
		throw AudioException("No more open ports available!");
	}
	if (jack_connect(client, jack_port_name(output_port_1), ports[0])) {
		delete ports;
		throw AudioException("Couldn't connect to output port!");
	}
	if (jack_connect(client, jack_port_name(output_port_2), ports[1])) {	//TODO Check if port 1 exists
		delete ports;
		throw AudioException("Couldn't connect to output port!");
	}

	delete ports;
	ports = NULL;
};

void AudioHandler::sample_rate_callback(jack_nframes_t nframes) {
	time_step = 1.0/nframes;
};

int AudioHandler::process(jack_nframes_t nframes) {
	jack_default_audio_sample_t* buffer1 = (jack_default_audio_sample_t*) jack_port_get_buffer(output_port_1, nframes);
	jack_default_audio_sample_t* buffer2 = (jack_default_audio_sample_t*) jack_port_get_buffer(output_port_2, nframes);

	//Compute each sample
	for (jack_nframes_t i = 0; i < nframes; ++i) {
		double sample = fmax(-1, fmin(1, get_sample(0, time, user_data)));
		jack_default_audio_sample_t frame = sample;

		buffer1[i] = frame;
		buffer2[i] = frame;

		time += time_step;
	}
	return 0;
};

void AudioHandler::close() {
	if (client != NULL) {
		jack_client_close(client);
		client = NULL;
	}
};

void AudioHandler::set_sample_callback(double (* get_sample) (unsigned int, double, void*), void* user_data) {
	this->get_sample = get_sample;
	this->user_data = user_data;
}

