/*
 * audio.h
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_AUDIO_H_
#define MIDICUBE_AUDIO_H_

#include <exception>
#include <jack/jack.h>

class AudioException : public std::exception {

private:
	const char* cause;

public:
	AudioException(char* cause) {
		this->cause = cause;
	}

	const char* what() const throw() {
		return cause;
	}
};


class AudioHandler {

private:
	jack_client_t* client = NULL;

public:

	~AudioHandler() {
		close();
	};

	void init();

	void close();

};

#endif /* MIDICUBE_AUDIO_H_ */
