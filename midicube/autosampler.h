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

class AutoSampler {
private:
	RtAudio audio;
	RtMidiOut rtmidi;

	unsigned int channel = 0;
	std::vector<unsigned int> notes;
	std::vector<unsigned int> velocities;

	unsigned int curr_note = 0;
	unsigned int curr_velocity = 0;

	std::atomic<bool> started = false;

public:

	void request_params();

	void init();

	inline int process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double time);

};



#endif /* MIDICUBE_AUTOSAMPLER_H_ */
