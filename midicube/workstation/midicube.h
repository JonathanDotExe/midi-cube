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
#include "midiio.h"
#include "../framework/core/audio.h"
#include "../framework/data/data.h"
#include "soundengine.h"
#include "../framework/util/audioloader.h"
#include "../framework/util/util.h"
#include "../framework/core/plugin.h"

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
	std::function<void(void*, void*)> property_callback;
	std::vector<MidiCubeInput> inputs;

	inline void process_midi(MidiMessage& message, size_t input);
public:
	std::array<MidiSource, SOUND_ENGINE_MIDI_CHANNELS> sources;
	size_t used_sources = 1;

	ProgramManager prog_mgr;
	SoundEngineDevice engine;
	PluginManager plugin_mgr;

	std::atomic<bool> updated{false};

	SpinLock lock;

	MidiCube(std::function<void(void*, void*)> c) : property_callback(c) {

	}

	void save_program(Program *prog);
	void apply_program(Program *prog);
	void notify_property_update(void* source, void* prop);
	MidiCube();
	void init(int out_device = -1, int in_device = -1);
	inline void process(double& lsample, double& rsample, double* inputs, const size_t input_count, SampleInfo& info);
	std::vector<MidiCubeInput> get_inputs();

	~MidiCube();
};


#endif /* MIDICUBE_MIDICUBE_H_ */
