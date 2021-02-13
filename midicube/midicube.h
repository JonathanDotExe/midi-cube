/*
 * midicube.h
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDICUBE_H_
#define MIDICUBE_MIDICUBE_H_

#include "audio.h"
#include "program.h"
#include "soundengine/soundengine.h"
#include "boost/lockfree/spsc_queue.hpp"


struct MidiCubeInput {
	MidiInput* in = nullptr;
	std::string name = "";
};

struct MidiMessageWithInput {
	size_t input = 0;
	MidiMessage msg;
};

class MidiCube {
private:
	AudioHandler audio_handler;
	ProgramManager prog_mgr;
	std::vector<MidiCubeInput> inputs;
	boost::lockfree::spsc_queue<PropertyChange> changes;
	boost::lockfree::spsc_queue<PropertyHolder*> update;
	boost::lockfree::spsc_queue<MidiMessageWithInput> messages;
	inline void process_midi(MidiMessage& message, size_t input);
public:
	SoundEngineDevice engine;

	MidiCube();
	void init(int out_device = -1, int in_device = -1);
	inline void process(double& lsample, double& rsample, SampleInfo& info);
	//Only call from audio thread
	void perform_change(PropertyChange change);
	//Only call from GUI thread
	void request_update(PropertyHolder* holder);
	std::vector<MidiCubeInput> get_inputs();
	//Only call from midi thread
	void midi_callback(MidiMessage& message, size_t input);
	~MidiCube();
};


#endif /* MIDICUBE_MIDICUBE_H_ */
