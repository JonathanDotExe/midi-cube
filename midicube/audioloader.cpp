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
			SNDFILE* file = nullptr;
			SF_INFO info;
			SF_INSTRUMENT loop;
			bool success = true;

			file = sf_open(req.sample->path.c_str(), SFM_READ, &info);

			//Read
			if (file != nullptr) {
				req.sample->lock.lock();
				if (!req.sample->loaded) {
					float buffer[1024];
					sf_count_t size = _countof(buffer);
					sf_count_t count;
					do {
						count = sf_read_float(file, buffer, size);
						for (sf_count_t i = 0; i < count; i++) {
							req.sample->samples.push_back(buffer[i]);
						}
					} while (count >= size);
					req.sample->loaded = true;
				}
				req.sample->lock.unlock();
			}
			else {
				std::cerr << "Couldn't open sound file " << req.sample->path << ": " << sf_strerror(NULL) << std::endl;
				success = false;
			}

			if (file != nullptr) {
				sf_close(file);
				file = nullptr;
			}
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
