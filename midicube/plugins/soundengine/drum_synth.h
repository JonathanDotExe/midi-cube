/*
 * drums.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_DRUM_SYNTH_H_
#define MIDICUBE_SOUNDENGINE_DRUM_SYNTH_H_

#include "../../framework/core/plugins/soundengine.h"
#include "../../framework/util/audiofile.h"
#include <unordered_map>

#define DRUM_SYNTH_IDENTIFIER "midicube_drum_synth"

class DrumSynthProgram : public PluginProgram {
public:
	virtual void load(boost::property_tree::ptree tree);
	virtual std::string get_plugin_name();
	virtual boost::property_tree::ptree save();
};

struct DrumSynthPreset {
	std::unordered_map<unsigned int, AudioSample> notes;
};

#define DRUM_SYNTH_POLYPHONY 30

class DrumSynth : public SoundEngine<TriggeredNote, DRUM_SYNTH_POLYPHONY> {

private:
	DrumSynthPreset preset;

public:

	DrumSynth(PluginHost& h, Plugin& p);

	void process_note_sample(const SampleInfo& info, TriggeredNote& note, size_t note_index);

	bool note_finished(const SampleInfo& info, TriggeredNote& note, size_t note_index);

	void apply_program(PluginProgram *prog);

	void save_program(PluginProgram **prog);

	~DrumSynth();
};


class DrumSynthPlugin : public Plugin {
public:
	DrumSynthPlugin() : Plugin({
		"Drum Synth",
		DRUM_SYNTH_IDENTIFIER,
		PluginType::PLUGIN_TYPE_SOUND_ENGINE,
		0,
		2,
		true,
		false
	}){

	}

	virtual PluginProgram* create_program();
	virtual PluginInstance* create(PluginHost *host);
	virtual ~DrumSynthPlugin() {

	}
};

#endif /* MIDICUBE_SOUNDENGINE_DRUM_SYNTH_H_ */
