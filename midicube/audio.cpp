/*
 * audio.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */
#include "audio.h"
#include <jack/jack.h>


class JackAudioHandler : public AudioHandler {

private:

	jack_client_t* client = NULL;

public:

	JackAudioHandler() {

	}

	~JackAudioHandler() {
		close();
	}

	void init() {
		jack_options_t options = JackNullOption;
		jack_status_t status;

		client = jack_client_open("MIDICube", options, &status, NULL);
		if (client == NULL) {
			throw AudioException("Couldn't connect to JACK Server!");
		}
	}

	void close() {
		if (client != NULL) {
			jack_client_close(client);
			client = NULL;
		}
	}



};

static AudioHandler* handler = NULL;


extern AudioHandler* create_audio_handler() {
	 if (handler == NULL) {
		 handler = new JackAudioHandler();
		 handler->init();
	 }
	 else {
		 throw AudioException("AudioHandler already created!");
	 }
	 return handler;
}

extern void dispose_audio_handler() {
	delete handler;
	handler = NULL;
}


