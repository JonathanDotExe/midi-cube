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
	StreamedAudioSample* sample = new StreamedAudioSample();
	read_stream_audio_file(*sample, fname);
	samples.push_back(sample);
	return sample;
}

void StreamedAudioPool::queue_request(LoadRequest request) {
	requests.push(request);
}

void StreamedAudioPool::run(bool gc) {
	using namespace std::chrono_literals;
	while (running) {
		LoadRequest req;
		if (requests.pop(req)) {
			//Assumes that files don't change
			//Open
			req.sample->lock.lock();
			req.sample->last_used = TIME_NOW();
			if (!req.sample->loaded) {
				SndfileHandle file(req.sample->path);
				req.sample->samples = std::vector<float>(req.sample->total_size * req.sample->channels, 0);
				file.read(&req.sample->samples[0], req.sample->samples.size());
				req.sample->loaded = true;
			}
			req.sample->lock.unlock();
		}
		else {
			if (gc) {
				//Garbage collect
				unsigned int time = TIME_NOW();
				gc_index %= samples.size();
				StreamedAudioSample* sample = samples[gc_index];
				if (sample->lock.try_lock()) {
					if (sample->loaded && sample->last_used + 20000 < time) {
						//Delete
						sample->samples.clear();
						sample->loaded = false;
					}
					sample->lock.unlock();
				}
				++gc_index;
			}
			std::this_thread::sleep_for(1ms);
		}
	}
}

void StreamedAudioPool::stop() {
	running = false;
}

void StreamedAudioPool::start() {
	for (size_t i = 0; i < 4; ++i) {
		std::thread thread([this, i]() { run(i == 0); });
		thread.detach();
	}
}
