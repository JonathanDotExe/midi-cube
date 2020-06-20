/*
 * audio.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */
#include "audio.h"
#include <jack/jack.h>
#include <math.h>

void sample_rate_callback(jack_nframes_t nframes, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;
	handler->time_step = 1.0/nframes;
}

int process(jack_nframes_t nframes, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;

	jack_default_audio_sample_t* buffer = jack_port_get_buffer(handler->output_port, nframes);
	//Compute each sample
	for (jack_nframes_t i = 0; i < nframes; ++i) {
		double sample = fmax(-1, fmin(1, handler->get_sample(0, handler->time)));
		buffer[i] = JACK_MAX_FRAMES/2 + (jack_nframes_t) (sample * JACK_MAX_FRAMES/2);
	}
	return 0;
}

void AudioHandler::AudioHandler() {

}

void AudioHandler::init() {
	jack_options_t options = JackNullOption;
	jack_status_t status;

	//Open client
	client = jack_client_open("MIDICube", options, &status, NULL);
	if (client == NULL) {
		throw AudioException("Couldn't connect to JACK Server!");
	}
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

void AudioHandler::close() {
	if (client != NULL) {
		jack_client_close(client);
		client = NULL;
	}
};

void AudioHandler::set_sample_callback(double (* get_sample) (unsigned int, double)) {
	this->get_sample = get_sample;
}

