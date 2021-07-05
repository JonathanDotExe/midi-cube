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
			//Open
			SNDFILE* file = nullptr;
			SF_INFO info;

			file = sf_open(req.sample->path.c_str(), SFM_READ, &info);

			//Read
			if (file != nullptr) {
				sf_count_t size = req.buffer->size;
				float* buffer = (*req.buffer)[req.buffer_index].buffer;
				memset((void *) buffer, 0, size * sizeof(buffer[0]));
				sf_read_float(file, buffer, size);
			}
			else {
				std::cerr << "Couldn't open sound file " << req.sample->path << ": " << sf_strerror(NULL) << std::endl;
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
