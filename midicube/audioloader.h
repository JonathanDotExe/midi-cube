/*
 * audioloader.h
 *
 *  Created on: Jul 4, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_AUDIOLOADER_H_
#define MIDICUBE_AUDIOLOADER_H_

#include "util.h"
#include "audiofile.h"
#include "boost/lockfree/spsc_queue.hpp"

struct LoadRequest {
	MultiBuffer<float>* buffer;
	size_t buffer_index;
	StreamedAudioSample* sample;
	size_t block;
};

class StreamedAudioLoader {
private:
	boost::lockfree::spsc_queue<LoadRequest> requests;
	std::atomic<bool> running{true};

public:

	StreamedAudioLoader() : requests(1024) {

	}

	//Audio Thread
	void queue_request(LoadRequest request);
	void run();
	void stop();
	void start();

	~StreamedAudioLoader() {
		stop();
	}

};



#endif /* MIDICUBE_AUDIOLOADER_H_ */
