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

#define TIME_NOW() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count()

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
	std::cout << "Started sample loading thread ..." << std::endl;
	while (running) {
		LoadRequest req;
		if (requests.pop(req)) {
			//Assumes that files don't change
			//Open
			req.sample->lock.lock();
			req.sample->last_used = TIME_NOW();
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
			if (gc) {
				//Garbage collect
				unsigned int time = TIME_NOW();
				gc_index %= samples.size();
				StreamedAudioSample& sample = samples[gc_index];
				if (sample.lock.try_lock()) {
					if (sample.last_used + 20000 < time) {
						//Delete
						sample.samples.clear();
						sample.loaded = false;
					}
					sample.lock.unlock();
				}
				++gc_index;
			}
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
