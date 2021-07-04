/*
 * audioloader.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: jojo
 */


#include "audioloader.h"


void StreamedAudioLoader::queue_request(LoadRequest request) {
	requests.push(request);
}

void StreamedAudioLoader::run() {
	while(running) {
		LoadRequest req;
		if (requests.pop(req)) {
			//TODO Load
		}
	}
}

void StreamedAudioLoader::stop() {
	running = false;
}
