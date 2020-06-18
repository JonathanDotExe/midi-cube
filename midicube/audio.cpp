/*
 * audio.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */
#include "audio.h"
#include <jack/jack.h>


void AudioHandler::init() {
	jack_options_t options = JackNullOption;
	jack_status_t status;

	client = jack_client_open("MIDICube", options, &status, NULL);
	if (client == NULL) {
		throw AudioException("Couldn't connect to JACK Server!");
	}
};

void AudioHandler::close() {
	if (client != NULL) {
		jack_client_close(client);
		client = NULL;
	}
};


