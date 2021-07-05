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


#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

void StreamedAudioLoader::queue_request(LoadRequest request) {
	requests.push(request);
}

void StreamedAudioLoader::run() {
	while(running) {
		LoadRequest req;
		if (requests.pop(req)) {
			//Assumes that file doesn't change over usage
			//Open
			SNDFILE* file = nullptr;
			SF_INFO info;

			file = sf_open(req.sample->path.c_str(), SFM_READ, &info);

			//Read
			if (file != nullptr) {
				if (sf_seek(file, req.sample->frames * req.block, SEEK_SET) != -1) {
					float* buffer = (*req.buffer)[req.buffer_index].buffer;
					(*req.buffer)[req.buffer_index].lock.lock();
					//memset((void *) buffer, 0, size * sizeof(buffer[0]));
					sf_readf_float(file, buffer, req.sample->frames);
					(*req.buffer)[req.buffer_index].lock.unlock();
				}
			}
			else {
				std::cerr << "Couldn't open sound file " << req.sample->path << ": " << sf_strerror(file) << std::endl;
			}

			if (file != nullptr) {
				sf_close(file);
				file = nullptr;
			}
		}
	}
}

void StreamedAudioLoader::stop() {
	running = false;
}
