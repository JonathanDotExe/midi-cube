/*
 * midicube.h
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDICUBE_H_
#define MIDICUBE_MIDICUBE_H_

#include "audio.h"
#include "soundengine/soundengine.h"
#include "boost/lockfree/queue.hpp"


struct MidiCubeInput {
	MidiInput* in = nullptr;
	std::string name = "";
};

class MidiCube {
private:
	AudioHandler audio_handler;
	std::vector<MidiCubeInput> inputs;
	boost::lockfree::queue<PropertyChange> changes;
	boost::lockfree::queue<PropertyHolder*> update;
public:
	SoundEngineDevice engine;

	MidiCube();
	void init(int out_device = -1, int in_device = -1);
	inline void process(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);
	void perform_change(PropertyChange change);
	void request_update(PropertyHolder* holder);
	std::vector<MidiCubeInput> get_inputs();
	void midi_callback(MidiMessage& message, size_t input);
	~MidiCube();
};


#endif /* MIDICUBE_MIDICUBE_H_ */
