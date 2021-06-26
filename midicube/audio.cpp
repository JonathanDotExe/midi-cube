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

int g_process(const void* input_buffer, void* output_buffer, long unsigned int buffer_size, const PaStreamCallbackTimeInfo* time, PaStreamCallbackFlags status, void* arg) {
	AudioHandler* handler = (AudioHandler*) arg;
	return handler->process((const float*) input_buffer, (float*) output_buffer, buffer_size);
}

void AudioHandler::init(int out_device, int in_device) {
	PaError paerr;
	paerr = Pa_Initialize();
	if (paerr != paNoError) {
		throw AudioException(Pa_GetErrorText(paerr));
	}
	const size_t device_count = Pa_GetDeviceCount();
	//List devices
	for (size_t i = 0; i < device_count; ++i) {
		const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
		std::cout << i << ": " << info->name << " " << info->maxOutputChannels << " outs " << info->maxInputChannels<< " ins" << std::endl;
	}
	if ((int) device_count <= std::max(std::max(out_device, in_device), 0)) {
		throw AudioException("No audio devices detected");
	}

	//Set up output
	PaStreamParameters params;
	params.device = out_device >= 0 ? out_device : Pa_GetDefaultOutputDevice();
	params.sampleFormat = paFloat32;
	const PaDeviceInfo* out_info = Pa_GetDeviceInfo(params.device);
	params.suggestedLatency = out_info->defaultLowOutputLatency;
	params.channelCount = 2;
	params.hostApiSpecificStreamInfo = NULL;

	//Set up input
	PaStreamParameters input_params;
	input_params.device = in_device >= 0 ? in_device : Pa_GetDefaultInputDevice();
	input_params.sampleFormat = paFloat32;
	const PaDeviceInfo* in_info = Pa_GetDeviceInfo(input_params.device);
	input_params.suggestedLatency = in_info->defaultLowInputLatency;
	input_params.channelCount = 1;
	input_params.hostApiSpecificStreamInfo = NULL;

	sample_rate = 44100;
	time_step = 1.0/sample_rate;
	buffer_size = 64;

	input = in_device >= 0;

	std::cout << "Using output device " << out_info->name << " with two channels" << std::endl;

	if (input) {
		std::cout << "Using input device " << in_info->name << " with one channels" << std::endl;
	}
	else {
		std::cout << "Using no input device" << std::endl;
	}

	paerr = Pa_OpenStream(&this->stream, input ? &input_params : NULL, &params, sample_rate, buffer_size, paNoFlag, &g_process, this);
	if (paerr != paNoError) {
		throw AudioException(Pa_GetErrorText(paerr));
	}
	PaAlsa_EnableRealtimeScheduling(stream, 90);
	paerr = Pa_StartStream(this->stream);
	if (paerr != paNoError) {
		throw AudioException(Pa_GetErrorText(paerr));
	}

	sample_rate = Pa_GetStreamInfo(this->stream)->sampleRate;
	time_step = 1.0/sample_rate;
	std::cout << "Opened audio stream Sample Rate: " << sample_rate << " Hz ... Buffer Size: " << buffer_size << std::endl;
};

int AudioHandler::process(const float* input_buffer, float* output_buffer, unsigned int buffer_size) {
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
	return paContinue;
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

