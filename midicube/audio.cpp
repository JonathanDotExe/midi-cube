/*
 * audio.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */
#include "audio.h"
#include <math.h>
#include <iostream>

#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

int g_process(void* output_buffer, void* input_buffer, unsigned int buffer_size, double time, RtAudioStreamStatus status, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;
	return handler->process((double*) output_buffer, (double*) input_buffer, buffer_size, time);
}

void AudioHandler::init() {
	if (audio.getDeviceCount() <= 0) {
		throw AudioException("No audio devices detected");
	}

	//Set up output
	RtAudio::StreamParameters params;
	params.deviceId = audio.getDefaultOutputDevice();
	params.nChannels = 2;
	params.firstChannel = 0;

	sample_rate = 44100;
	time_step = 1.0/sample_rate;
	buffer_size = 256;

	try {
		audio.openStream(&params, nullptr, RTAUDIO_FLOAT64, sample_rate, &buffer_size, &g_process, this);
		audio.startStream();
	}
	catch (RtAudioError& e) {
		throw AudioException(e.getMessage().c_str());
	}
};

int AudioHandler::process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double time) {
	SampleInfo info;
	//Compute each sample
	//TODO Use time
	for (size_t i = 0; i < buffer_size; ++i) {
		info = {time, time_step, sample_rate, sample_time, 0}; //TODO input

		sample_buf = {0, 0};
		get_sample(sample_buf, info, user_data);

		//TODO array safety
		*output_buffer++ = sample_buf[0];
		*output_buffer++ = sample_buf[1];

		time = time + time_step;
		++sample_time;
	}
	return 0;
};

void AudioHandler::close() {
	if (audio.isStreamOpen()) {
		audio.closeStream();
	}
};

void AudioHandler::set_sample_callback(void (* get_sample) (std::array<double, OUTPUT_CHANNELS>&, SampleInfo&, void*), void* user_data) {
	this->get_sample = get_sample;
	this->user_data = user_data;
}

SampleInfo AudioHandler::sample_info() {
	SampleInfo info;
	info.time = time;
	info.time_step = time_step;
	info.sample_rate = sample_rate;
	info.sample_time = sample_time;
	return info;
}

