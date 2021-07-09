/*
 * audioloader.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: jojo
 */


#include "audioloader.h"

#include <sndfile.h>
#include <sndfile.hh>
#include <cstring>
#include <chrono>




#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

StreamedAudioSample* StreamedAudioPool::load_sample(std::string fname) {
	samples.push_back({});
	StreamedAudioSample& sample = samples[samples.size() - 1];
	read_stream_audio_file(sample, fname);
	return sample;
}

void StreamedAudioPool::queue_request(LoadRequest request) {
	requests.push(request);
}

void StreamedAudioPool::run() {
	using namespace std::chrono_literals;
	std::cout << "Started sample loading thread ..." << std::endl;
	while (running) {
		LoadRequest req;
		if (requests.pop(req)) {
			//Assumes that files don't change
			//Open
			req.sample->lock.lock();
			if (!req.sample->loaded) {
				SndfileHandle file(req.sample->path);
				req.sample->samples.clear();
				req.sample->samples.reserve(file.frames());
				file.read(&req.sample->samples[0], file.frames());
				req.sample->loaded = true;
			}
			req.sample->lock.unlock();
		}
		else {
			std::this_thread::sleep_for(1ms);
		}
	}
}

void StreamedAudioPool::stop() {
	running = false;
}

void StreamedAudioPool::start() {
	for (size_t i = 0; i < 4; ++i) {
		std::thread thread([this]() { run(); });
		thread.detach();
	}
}
