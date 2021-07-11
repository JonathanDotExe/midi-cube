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
#include "boost/lockfree/queue.hpp"
#include <thread>


struct LoadRequest {
	StreamedAudioSample* sample;
};

class StreamedAudioPool {
private:
	std::vector<StreamedAudioSample*> samples;
	boost::lockfree::queue<LoadRequest> requests;
	std::atomic<bool> running{true};
	std::array<std::thread, 4> worker;
	size_t gc_index = 0;

public:

	StreamedAudioSample* load_sample(std::string fname);
	void queue_request(LoadRequest request);
	void run(bool gc);
	void stop();
	void start();

	~StreamedAudioPool() {
		stop();
		for (StreamedAudioSample* sample : samples) {
			delete sample;
		}
		samples.clear();
	}
};

#endif /* MIDICUBE_AUDIOLOADER_H_ */
