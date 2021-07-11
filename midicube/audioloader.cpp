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
	std::cout << "Queued request " << request.sample->path << std::endl;
	requests.push(request);
}

void StreamedAudioPool::run(bool gc) {
	using namespace std::chrono_literals;
	std::cout << "Started sample loading thread ..." << std::endl;
	while (running) {
		LoadRequest req;
		if (requests.pop(req)) {
			std::cout << "Popped request " << req.sample->path << std::endl;
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
			std::cout << "Loaded " << req.sample->path << std::endl;
		}
		else {
			std::this_thread::sleep_for(1ms);
			if (gc) {
				//Garbage collect
				unsigned int time = TIME_NOW();
				gc_index %= samples.size();
				StreamedAudioSample* sample = samples[gc_index];
				if (sample->lock.try_lock()) {
					if (sample->loaded && sample->last_used + 60000 < time) {
						//Delete
						sample->samples.clear();
						sample->loaded = false;
						std::cout << "Garbage collected " << sample->path << std::endl;
					}
					sample->lock.unlock();
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
