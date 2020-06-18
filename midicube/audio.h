/*
 * audio.h
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_AUDIO_H_
#define MIDICUBE_AUDIO_H_

#include <exception>

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

public:
	virtual void init() = 0;
	virtual void close() = 0;

	virtual ~AudioHandler() {

	}

};

extern AudioHandler* create_audio_handler();

extern void dispose_audio_handler();

#endif /* MIDICUBE_AUDIO_H_ */
