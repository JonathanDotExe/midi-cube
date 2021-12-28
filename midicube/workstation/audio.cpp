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


static RtAudio::Api get_api(std::string driver) {
	boost::to_upper(driver);
	if (driver == "ALSA") {
		return RtAudio::Api::LINUX_ALSA;
	}
	else if (driver == "PULSE") {
		return RtAudio::Api::LINUX_PULSE;
	}
	else if (driver == "OSS") {
		return RtAudio::Api::LINUX_OSS;
	}
	else if (driver == "JACK") {
		return RtAudio::Api::UNIX_JACK;
	}
	else if (driver == "CORE") {
		return RtAudio::Api::MACOSX_CORE;
	}
	else if (driver == "WASAPI") {
		return RtAudio::Api::WINDOWS_WASAPI;
	}
	else if (driver == "ASIO") {
		return RtAudio::Api::WINDOWS_ASIO;
	}
	else if (driver == "DS") {
		return RtAudio::Api::WINDOWS_DS;
	}
	else if (driver == "DUMMY") {
		return RtAudio::Api::RTAUDIO_DUMMY;
	}
	return RtAudio::Api::UNSPECIFIED;
}


void AudioHandler::init(std::string driver, unsigned int sample_rate, unsigned int buffer_size, int out_device, int in_device, int input_amount) {
	audio = new RtAudio(get_api(driver));
	
	if ((int) audio->getDeviceCount() <= std::max(std::max(out_device, in_device), 0)) {
		throw AudioException("No audio devices detected");
	}
	//List devices
	for (size_t i = 0; i < audio->getDeviceCount(); ++i) {
		RtAudio::DeviceInfo info = audio->getDeviceInfo(i);
		std::cout << i << ": " << info.name << " " << info.outputChannels << " outs " << info.inputChannels << " ins" << std::endl;
	}

	//Set up options
	RtAudio::StreamOptions options;
	options.flags = options.flags | RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_HOG_DEVICE;
	options.priority = 90;

	//Set up output
	RtAudio::StreamParameters params;
	params.deviceId = out_device >= 0 ? out_device : audio->getDefaultOutputDevice();
	params.nChannels = 2;
	params.firstChannel = 0;

	//Set up input
	RtAudio::StreamParameters input_params;
	input_params.deviceId = in_device >= 0 ? in_device : audio->getDefaultInputDevice();
	input_params.nChannels = input_amount;
	input_params.firstChannel = 0;

	this->sample_rate = sample_rate;
	time_step = 1.0/this->sample_rate;
	this->buffer_size = buffer_size;

	inputs = input_amount;

	RtAudio::DeviceInfo out_info = audio->getDeviceInfo(params.deviceId);
	std::cout << "Using output device " << out_info.name << " with two channels" << std::endl;

	if (inputs) {
		RtAudio::DeviceInfo in_info = audio->getDeviceInfo(params.deviceId);
		std::cout << "Using input device " << in_info.name << " with one channels" << std::endl;
	}
	else {
		std::cout << "Using no input device" << std::endl;
	}

	try {
		audio->openStream(&params, inputs ? &input_params : nullptr, RTAUDIO_FLOAT64, this->sample_rate, &this->buffer_size, &g_process, this);
		audio->startStream();
		this->sample_rate = audio->getStreamSampleRate();
		time_step = 1.0/this->sample_rate;
		std::cout << "Opened audio stream Sample Rate: " << this->sample_rate << " Hz ... Buffer Size: " << this->buffer_size << std::endl;
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
		info = {time, time_step, sample_rate, sample_time};

		double lsample = 0;
		double rsample = 0;
		get_sample(lsample, rsample, input_buffer, inputs, info, user_data);

		for (size_t i = 0; i < inputs; ++i) {
			++input_buffer;
		}

		*output_buffer++ = lsample;
		*output_buffer++ = rsample;

		this->time += time_step;
		++sample_time;
	}
	return 0;
};

void AudioHandler::close() {
	if (audio && audio->isStreamOpen()) {
		audio->closeStream();
	}
	delete audio;
	audio = nullptr;
};

void AudioHandler::set_sample_callback(void (* get_sample) (double&, double&, double*, const size_t, SampleInfo&, void*), void* user_data) {
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

