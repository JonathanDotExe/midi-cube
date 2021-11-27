/*
 * audio.h
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_AUDIO_H_
#define MIDICUBE_AUDIO_H_

#include <exception>
#include <atomic>
#include <array>
#include <rtaudio/RtAudio.h>
#include <boost/algorithm/string.hpp>
#include "../framework/core/midi.h"
#include "../framework/core/audio.h"

class AudioException : public std::exception {

private:
	const char* cause;

public:
	AudioException(const char* cause) {
		this->cause = cause;
	}

	const char* what() const throw() {
		return cause;
	}
};

static RtAudio::Api get_api(std::string driver) {
	boost::to_upper(driver);
	if (driver == "ALSA") {
		return RtAudio::Api::LINUX_ALSA;
	}
	else if (driver == "PULSE") {
		return RtAudio::Api::LINUX_PULSE;
	}
	else if (driver == "OSS") {
		return RtAudio::Api::LINUX_OSS;
	}
	else if (driver == "JACK") {
		return RtAudio::Api::UNIX_JACK;
	}
	else if (driver == "CORE") {
		return RtAudio::Api::MACOSX_CORE;
	}
	else if (driver == "WASAPI") {
		return RtAudio::Api::WINDOWS_WASAPI;
	}
	else if (driver == "ASIO") {
		return RtAudio::Api::WINDOWS_ASIO;
	}
	else if (driver == "DS") {
		return RtAudio::Api::WINDOWS_DS;
	}
	else if (driver == "DUMMY") {
		return RtAudio::Api::RTAUDIO_DUMMY;
	}
	return RtAudio::Api::UNSPECIFIED;
}

class AudioHandler {

private:
	RtAudio* audio = nullptr;
	size_t inputs = 0;
	void* user_data = nullptr;
	void (* get_sample) (double&, double&, double*, const size_t, SampleInfo&, void*) = nullptr;
	/**
	 * Only for use in the jack audio thread
	 */
	unsigned int buffer_size = 256;
	double time{0.0};
	double time_step{0.0};
	unsigned int sample_rate{0};
	unsigned int sample_time{0};
public:

	AudioHandler()  {

	}

	~AudioHandler() {
		close();
	};

	void set_sample_callback(void (* get_sample) (double&, double&, double*, const size_t, SampleInfo&, void*), void* user_data);

	void init(std::string driver, unsigned int sample_rate, unsigned int buffer_size, int out_device, int in_device, int input_amount);

	void close();

	int process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double time);

	SampleInfo sample_info();

};

#endif /* MIDICUBE_AUDIO_H_ */
