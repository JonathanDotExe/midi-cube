/*
 * autosampler.cpp
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */

#include "autosampler.h"

void AutoSampler::request_params() {
	std::cout << "Welcome to the Auto-Sample tool for MIDICube!" << std::endl;
	//MIDI device
	const unsigned int ports = rtmidi.getPortCount();
	for (unsigned int i = 0; i < ports; ++i) {
		std::cout << i << ": " << rtmidi.getPortName(i) << std::endl;
	}
	std::cout << "Which MIDI device should be sampled?" << std::endl;
	std::cin >> midi_device;

	//Audio device
	const unsigned int a_ports = rtaudio.getDeviceCount();
	for (unsigned int i = 0; i < a_ports; ++i) {
		std::cout << i << ": " << rtaudio.getDeviceInfo(i).name << std::endl;
	}
	std::cout << "Which audio device should be sampled?" << std::endl;
	std::cin >> audio_device;

	//Velocities
	int vel = 0;
	while (vel >= 0) {
		std::cout << "Which velocity should be sampled? (-1 to stop)" << std::endl;
		std::cin >> vel;
		if (vel >= 0) {
			velocities.push_back(vel);
		}
	}

	//Notes
	unsigned int note = 0;
	unsigned int end_note = 0;
	unsigned int step = 0;
	std::cout << "Which note do you want to start with?" << std::endl;
	std::cin >> note;
	std::cout << "Which note do you want to end with?" << std::endl;
	std::cin >> end_note;
	std::cout << "Which steps should be between the notes?" << std::endl;
	std::cin >> step;
	for (; note <= end_note; note += step) {
		notes.push_back(note);
	}
}

void AutoSampler::init() {

}

inline int AutoSampler::process(double *output_buffer, double *input_buffer,
		unsigned int buffer_size, double time) {

	return 0;
}
