/*
 * autosampler.cpp
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */

#include "autosampler.h"

#define MAX_QUIET_TIME 44100

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

int sampler_process(void* output_buffer, void* input_buffer, unsigned int buffer_size, double time, RtAudioStreamStatus status, void* arg) {
	AutoSampler* handler = (AutoSampler*) arg;
	return handler->process((double*) output_buffer, (double*) input_buffer, buffer_size, time);
}


void AutoSampler::init() {
	//Open midi
	std::cout << "Opening port " << rtmidi.getPortName(midi_device) << std::endl;
	rtmidi.openPort(midi_device);

	//Open audio
	//Set up options
	RtAudio::StreamOptions options;
	options.flags = options.flags | RTAUDIO_SCHEDULE_REALTIME;
	options.priority = 90;

	//Set up input
	RtAudio::StreamParameters input_params;
	input_params.deviceId = audio_device;
	input_params.nChannels = 2;
	input_params.firstChannel = 0;

	double sample_rate = 44100;
	unsigned int buffer_size = 256;

	rtaudio.openStream(nullptr, &input_params, RTAUDIO_FLOAT64, sample_rate, &buffer_size, &sampler_process, this);
	rtaudio.startStream();
	sample_rate = rtaudio.getStreamSampleRate();
	std::cout << "Opened audio stream Sample Rate: " << sample_rate << " Hz ... Buffer Size: " << buffer_size << std::endl;
}

inline int AutoSampler::process(double *output_buffer, double *input_buffer,
		unsigned int buffer_size, double time) {

	for (size_t i = 0; i < buffer_size; ++i) {
		//Press note
		if (!pressed && curr_note < notes.size() && curr_velocity < velocities.size()) {
			//First note
			unsigned char status = 0x90 | ((char) channel & 0x0F);
			std::vector<unsigned char> msg = {status, (unsigned char) notes.at(curr_note), (unsigned char) notes.at(curr_velocity)};
			rtmidi.sendMessage(&msg);

			std::cout << "Sampling note " << notes.at(curr_note) << " at velocity " << velocities.at(curr_velocity) << "!" << std::endl;
			pressed = true;
		}

		if (pressed) {
			double l = *input_buffer++;
			double r = *input_buffer++;

			//Check if audio started
			if (!started_audio && (l || r)) {
				started_audio = true;
			}

			//Process signal
			if (started_audio) {
				lsample.push_back(l);
				rsample.push_back(r);

				//Update last signal time
				if ((l || r)) {
					last_signal_time = lsample.size() - 1;
				}

				//Check end
				if (last_signal_time + MAX_QUIET_TIME < time) {
					started_audio = false;
					//TODO save
					lsample.clear();
					rsample.clear();
					//Release old note
					unsigned char status = 0x80 | ((char) channel & 0x0F);
					std::vector<unsigned char> msg = {status, (unsigned char) notes.at(curr_note), 0};

					std::cout << "Finished sampling note " << notes.at(curr_note) << " at velocity " << velocities.at(curr_velocity) << "!" << std::endl;

					rtmidi.sendMessage(&msg);
					pressed = false;

					//Find next note/velocity
					curr_note++;
					if (curr_note >= notes.size()) {
						curr_note = 0;
						curr_velocity++;
					}
				}
			}
		}
	}
	return 0;
}
