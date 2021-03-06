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
#include <mutex>


struct MidiCubeInput {
	MidiInput* in = nullptr;
	std::string name = "";
};

struct MidiMessageWithInput {
	size_t input = 0;
	MidiMessage msg;
};

class MidiCube : public ProgramUser {
private:
	AudioHandler audio_handler;
	std::vector<MidiCubeInput> inputs;
	boost::lockfree::spsc_queue<PropertyChange> changes;
	boost::lockfree::spsc_queue<PropertyHolder*> update;
	//Only for sync program changes
	std::mutex mutex;

	inline void process_midi(MidiMessage& message, size_t input);
public:
	ProgramManager prog_mgr;
	SoundEngineDevice engine;

	virtual void save_program(Program *prog);
	virtual void apply_program(Program *prog);
	MidiCube();
	void init(int out_device = -1, int in_device = -1);
	inline void process(double& lsample, double& rsample, SampleInfo& info);
	//Only call from audio thread
	void perform_change(PropertyChange change);
	//Only call from GUI thread
	void request_update(PropertyHolder* holder);
	std::vector<MidiCubeInput> get_inputs();
	~MidiCube();
};


#endif /* MIDICUBE_MIDICUBE_H_ */
