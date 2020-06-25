/*
 * midi-cube.h
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDICUBE_H_
#define MIDICUBE_MIDICUBE_H_

#include <unordered_map>

#include "audio.h"
#include "device.h"

class MidiCube {
private:
	AudioHandler audio_handler;
	std::unordered_map<std::string, AudioDevice*> devices;
public:
	MidiCube();
	~MidiCube();
	void init();
	void create_default_devices();
	void add_device(AudioDevice* device);
	double process(unsigned int channel, double time);
};

#endif /* MIDICUBE_MIDICUBE_H_ */
