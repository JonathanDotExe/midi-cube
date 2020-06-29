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
#include <atomic>

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
};

class AudioHandler {

private:
	jack_client_t* client = NULL;
	jack_port_t* output_port_1 = NULL;
	jack_port_t* output_port_2 = NULL;
	void* user_data = nullptr;
	double (* get_sample) (unsigned int, SampleInfo&, void*) = NULL;
	/**
	 * Only for use in the jack audio thread
	 */
	double time = 0.0;
	std::atomic<double> time_step{0.0};
	std::atomic<unsigned int> sample_rate{0};
	unsigned int sample_time = 0;
public:

	~AudioHandler() {
		close();
	};

	void set_sample_callback(double (* get_sample) (unsigned int, SampleInfo&, void*), void* user_data);

	void init();

	void close();

	int process(jack_nframes_t nframes);

	void sample_rate_callback(jack_nframes_t nframes);

};

#endif /* MIDICUBE_AUDIO_H_ */
