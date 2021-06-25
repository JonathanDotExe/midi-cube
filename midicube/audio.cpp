/*
 * audio.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: jojo
 */
#include "audio.h"
#include <pa_linux_alsa.h>
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
	const size_t device_count = Pa_GetDeviceCount();
	if ((int) device_count <= std::max(std::max(out_device, in_device), 0)) {
		throw AudioException("No audio devices detected");
	}
	//List devices
	for (size_t i = 0; i < device_count; ++i) {
		PaDeviceInfo* info = Pa_GetDeviceInfo(i);
		std::cout << i << ": " << info->name << " " << info->maxOutputChannels << " outs " << info->maxInputChannels<< " ins" << std::endl;
	}

	//Set up output
	PaStreamParameters params;
	params.device = out_device >= 0 ? out_device : Pa_GetDefaultOutputDevice();
	PaDeviceInfo* out_info = Pa_GetDeviceInfo(params.device);
	params.suggestedLatency = out_info->defaultLowOutputLatency;
	params.channelCount = 2;

	//Set up input
	PaStreamParameters input_params;
	input_params.device = in_device >= 0 ? in_device : Pa_GetDefaultInputDevice();
	PaDeviceInfo* in_info = Pa_GetDeviceInfo(input_params.device);
	input_params.suggestedLatency = in_info->defaultLowInputLatency;
	input_params.channelCount = 1;

	sample_rate = 44100;
	time_step = 1.0/sample_rate;
	buffer_size = 256;

	input = in_device >= 0;

	std::cout << "Using output device " << out_info->name << " with two channels" << std::endl;

	if (input) {
		std::cout << "Using input device " << in_info->name << " with one channels" << std::endl;
	}
	else {
		std::cout << "Using no input device" << std::endl;
	}

	if (Pa_Initialize() != paNoError) {
		throw AudioException("Couldn't init portaudio");
	}
	if (Pa_OpenStream(&stream, input ? &input_params : nullptr, &params, sample_rate, buffer_size, 0, &g_process, this) != paNoError) {
		throw AudioException("Couldn't open stream");
	}
	PaAlsa_EnableRealtimeScheduling(&stream, 90);
	if (Pa_StartStream(&stream) != paNoError) {
		throw AudioException("Couldn't start stream");
	}

	sample_rate = Pa_GetStreamInfo(stream)->sampleRate;
	time_step = 1.0/sample_rate;
	std::cout << "Opened audio stream Sample Rate: " << sample_rate << " Hz ... Buffer Size: " << buffer_size << std::endl;
};

int AudioHandler::process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double t) {
	SampleInfo info;
	//Compute each sample
	for (size_t i = 0; i < buffer_size; ++i) {
		double in = input ? *input_buffer++ : 0;
		info = {time, time_step, sample_rate, sample_time, in};

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
	Pa_CloseStream(stream);
	Pa_Terminate();
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

