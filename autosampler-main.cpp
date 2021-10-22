/*
 * autosampler.cpp
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */


#include <iostream>
#include <thread>
#include <chrono>

#include "midicube/workstation/autosampler.h"


int main(int argc, char **argv) {
	std::cout << "Welcome to the MIDICube sample utils! What dou you want to do?" << std::endl;
	std::cout << "1: Record samples from a MIDI device" << std::endl;
	std::cout << "2: Configure a sound using recorded samples" << std::endl;
	std::cout << "3: Convert an sfz file to a midi cube sample" << std::endl;
	int in = 0;
	std::cin >> in;
	switch (in) {
	case 1:
	{
		AutoSampler sampler;

		sampler.request_params();
		sampler.init();

		while (sampler.running) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	break;
	case 2:
	{
		SampleSoundCreator sampler;

		sampler.request_params();
		sampler.generate_sound();
	}
	break;
	case 3:
	{
		SfzSampleConverter conv;
		conv.request_params();
		conv.convert();
	}
	break;
	}

	return 0;
}
