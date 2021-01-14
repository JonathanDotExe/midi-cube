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

#define OUTPUT_CHANNELS 2

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

struct SampleInfo {
	double time;
	double time_step;
	unsigned int sample_rate;
	unsigned int sample_time;
	double input_sample;
};

class AudioHandler {

private:
	RtAudio audio;
	void* user_data = nullptr;
	void (* get_sample) (std::array<double, OUTPUT_CHANNELS>&, SampleInfo&, void*) = nullptr;
	/**
	 * Only for use in the jack audio thread
	 */
	unsigned int buffer_size = 256;
	double time{0.0};
	double time_step{0.0};
	unsigned int sample_rate{0};
	unsigned int sample_time{0};
	std::array<double, OUTPUT_CHANNELS> sample_buf = {};
public:

	~AudioHandler() {
		close();
	};

	void set_sample_callback(void (* get_sample) (std::array<double, OUTPUT_CHANNELS>&, SampleInfo&, void*), void* user_data);

	void init(int device = -1);

	void close();

	int process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double time);

	SampleInfo sample_info();

};

#endif /* MIDICUBE_AUDIO_H_ */
