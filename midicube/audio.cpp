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

void AudioHandler::init(int out_device, int in_device) {
	if ((int) audio.getDeviceCount() <= std::max(std::max(out_device, in_device), 0)) {
		throw AudioException("No audio devices detected");
	}
	//List devices
	for (size_t i = 0; i < audio.getDeviceCount(); ++i) {
		RtAudio::DeviceInfo info = audio.getDeviceInfo(i);
		std::cout << i << ": " << info.name << " " << info.outputChannels << " outs " << info.inputChannels << " ins" << std::endl;
	}

	//Set up options
	RtAudio::StreamOptions options;
	options.flags = options.flags | RTAUDIO_SCHEDULE_REALTIME;
	options.priority = 90;

	//Set up output
	RtAudio::StreamParameters params;
	params.deviceId = out_device >= 0 ? out_device : audio.getDefaultOutputDevice();
	params.nChannels = 2;
	params.firstChannel = 0;

	//Set up input
	RtAudio::StreamParameters input_params;
	input_params.deviceId = in_device >= 0 ? in_device : audio.getDefaultInputDevice();
	input_params.nChannels = 2;
	input_params.firstChannel = 0;

	sample_rate = 48000;
	time_step = 1.0/sample_rate;
	buffer_size = 64;

	input = in_device >= 0;

	RtAudio::DeviceInfo out_info = audio.getDeviceInfo(params.deviceId);
	std::cout << "Using output device " << out_info.name << " with two channels" << std::endl;

	if (input) {
		RtAudio::DeviceInfo in_info = audio.getDeviceInfo(params.deviceId);
		std::cout << "Using input device " << in_info.name << " with one channels" << std::endl;
	}
	else {
		std::cout << "Using no input device" << std::endl;
	}

	try {
		audio.openStream(&params, input ? &input_params : nullptr, RTAUDIO_FLOAT64, sample_rate, &buffer_size, &g_process, this);
		audio.startStream();
		sample_rate = audio.getStreamSampleRate();
		time_step = 1.0/sample_rate;
		std::cout << "Opened audio stream Sample Rate: " << sample_rate << " Hz ... Buffer Size: " << buffer_size << std::endl;
	}
	catch (RtAudioError& e) {
		throw AudioException(e.getMessage().c_str());
	}
};

int AudioHandler::process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double t) {
	SampleInfo info;
	//Compute each sample
	//TODO Use rtaudio time
	for (size_t i = 0; i < buffer_size; ++i) {
		double in = 0;
		if (input) {
			in += (*input_buffer++);
			in += (*input_buffer++);
		}
		info = {time, time_step, sample_rate, sample_time};

		double lsample = 0;
		double rsample = 0;
		get_sample(lsample, rsample, info, user_data);

		//TODO array safety
		*output_buffer++ = lsample;
		*output_buffer++ = rsample;

		this->time += time_step;
		++sample_time;
	}
	return 0;
};

void AudioHandler::close() {
	if (audio.isStreamOpen()) {
		audio.closeStream();
	}
};

void AudioHandler::set_sample_callback(void (* get_sample) (double&, double&, SampleInfo&, void*), void* user_data) {
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

