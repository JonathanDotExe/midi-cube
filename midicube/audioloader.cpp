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

StreamedAudioLoader global_audio_loader;

void StreamedAudioLoader::queue_request(LoadRequest request) {
	requests.push(request);
	std::cout << "Requested loading block " << request.block << " in " << request.buffer_index << " of " << request.sample->path << std::endl;
}

void StreamedAudioLoader::run() {
	using namespace std::chrono_literals;
	std::cout << "Started loading thread" << std::endl;
	while (running) {
		LoadRequest req;
		if (requests.pop(req)) {
			std::cout << "Popping request" << std::endl;
			(*req.buffer)[req.buffer_index].lock.lock();
			std::cout << "Started loading block" << std::endl;
			if ((*req.buffer)[req.buffer_index].content_id != req.block) {
				//Assumes that file doesn't change over usage
				//Open
				SNDFILE* file = nullptr;
				SF_INFO info;

				file = sf_open(req.sample->path.c_str(), SFM_READ, &info);

				//Read
				if (file != nullptr) {
					size_t frames = req.buffer->size;
					if (sf_seek(file, frames * req.block, SF_SEEK_SET) != -1) {
						float* buffer = (*req.buffer)[req.buffer_index].buffer;
						//memset((void *) buffer, 0, size * sizeof(buffer[0]));
						sf_readf_float(file, buffer, frames);	//It must be externally ensured that all the frames fit in the buffer
						(*req.buffer)[req.buffer_index].content_id = req.block;
					}
				}
				else {
					std::cerr << "Couldn't open sound file " << req.sample->path << ": " << sf_strerror(file) << std::endl;
				}

				if (file != nullptr) {
					sf_close(file);
					file = nullptr;
				}
				(*req.buffer)[req.buffer_index].lock.unlock();
				std::cout << "Loaded block " << req.block << " in " << req.buffer_index << " of " << req.sample->path << std::endl;
			}
		}
		else {
			std::this_thread::sleep_for(1ms);
		}
	}
}

void StreamedAudioLoader::start() {
	for (size_t i = 0; i < 4; ++i) {
		std::thread thread([this]() { run(); });
		thread.detach();
	}
}

void StreamedAudioLoader::stop() {
	running = false;
}
