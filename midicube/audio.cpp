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

int g_sample_rate_callback(jack_nframes_t nframes, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;
	handler->sample_rate_callback(nframes);
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
	//Create output port
	output_port = jack_port_register(client, "MIDICube Sound Output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	if (output_port == NULL) {
		throw AudioException("Couldn't create output port");
	}
	//Start callback
	if (jack_activate(client)) {
		throw AudioException("Couldn't activate JACK client!");
	}
	//Connect output port
	const char** ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
	if (ports == NULL) {
		throw AudioException("No more open port available!");
	}
	if (jack_connect(client, jack_port_name(output_port), ports[0])) {
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
	jack_default_audio_sample_t* buffer = (jack_default_audio_sample_t*) jack_port_get_buffer(output_port, nframes);
	//Compute each sample
	for (jack_nframes_t i = 0; i < nframes; ++i) {
		double sample = fmax(-1, fmin(1, get_sample(0, time)));
		buffer[i] = JACK_MAX_FRAMES/2 + (jack_nframes_t) (sample * JACK_MAX_FRAMES/2);
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

void AudioHandler::set_sample_callback(double (* get_sample) (unsigned int, double)) {
	this->get_sample = get_sample;
}

