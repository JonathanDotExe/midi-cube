/*
 * autosampler.cpp
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */


#include <iostream>
#include <thread>
#include <chrono>

#include "midicube/autosampler.h"


int main(int argc, char **argv) {
	AutoSampler sampler;

	sampler.request_params();
	sampler.init();

	while (sampler.running) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}

