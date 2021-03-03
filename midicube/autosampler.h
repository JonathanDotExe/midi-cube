/*
 * autosampler.h
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_AUTOSAMPLER_H_
#define MIDICUBE_AUTOSAMPLER_H_

#include <iostream>
#include <vector>
#include <atomic>
#include <array>
#include <string>
#include <rtmidi/RtMidi.h>
#include <rtaudio/RtAudio.h>

#include "audiofile.h"

#define AUTOSAMPLER_CHANNELS 2

class AutoSampler {
private:
	RtAudio rtaudio;
	RtMidiOut rtmidi;

	unsigned int channel = 0;
	std::vector<unsigned int> notes;
	std::vector<unsigned int> velocities;

	unsigned int audio_device = 0;
	unsigned int midi_device = 0;

	unsigned int sample_rate = 44100;

	size_t curr_note = 0;
	size_t curr_velocity = 0;

	bool started_audio = false;
	bool pressed = false;
	double last_signal_time = 0;

	AudioSample sample;

public:

	std::atomic<bool> running{true};

	AutoSampler() {

	}

	void request_params();

	void init();

	inline int process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double time);

	~AutoSampler() {
		rtaudio.closeStream();
		rtmidi.closePort();
	}

};



#endif /* MIDICUBE_AUTOSAMPLER_H_ */
