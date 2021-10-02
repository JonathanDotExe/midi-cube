/*
 * drums.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_DRUMS_H_
#define MIDICUBE_SOUNDENGINE_DRUMS_H_

#include "../framework/core/plugins/soundengine.h"
#include "../framework/util/audiofile.h"
#include <unordered_map>

#define SAMPLE_DRUMS_IDENTIFIER "midicube_sample_drums"

class SampleDrumsProgram : public PluginProgram {
public:
	virtual void load(boost::property_tree::ptree tree);
	virtual std::string get_plugin_name();
	virtual boost::property_tree::ptree save();
};

struct SampleDrumKit {
	std::unordered_map<unsigned int, AudioSample> notes;
};

#define SAMPLE_DRUMS_POLYPHONY 30

class SampleDrums : public SoundEngine<TriggeredNote, SAMPLE_DRUMS_POLYPHONY> {

private:
	SampleDrumKit* drumkit;

public:

	SampleDrums(PluginHost& h, Plugin& p);

	void process_note_sample(const SampleInfo& info, TriggeredNote& note, size_t note_index);

	bool note_finished(const SampleInfo& info, TriggeredNote& note, size_t note_index);

	void apply_program(PluginProgram *prog);

	void save_program(PluginProgram **prog);

	~SampleDrums();
};

extern SampleDrumKit* load_drumkit(std::string folder);

class SampleDrumsPlugin : public Plugin {
public:
	SampleDrumsPlugin() : Plugin({
		"Sample Drums",
		SAMPLE_DRUMS_IDENTIFIER,
		0,
		2,
		true,
		false
	}){

	}

	virtual PluginProgram* create_program();
	virtual PluginInstance* create(PluginHost *host);
	virtual ~SampleDrumsPlugin() {

	}
};

#endif /* MIDICUBE_SOUNDENGINE_DRUMS_H_ */
