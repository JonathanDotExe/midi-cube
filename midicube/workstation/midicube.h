/*
 * midicube.h
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDICUBE_H_
#define MIDICUBE_MIDICUBE_H_

#include <atomic>
#include "audio.h"
#include "program.h"
#include "midiio.h"
#include "../framework/core/audio.h"
#include "../framework/data/data.h"
#include "soundengine.h"
#include "../framework/util/audioloader.h"
#include "../framework/util/util.h"
#include "../framework/util/clipboard.h"
#include "../framework/core/plugin.h"
#include "../framework/core/ui.h"

struct MidiCubeInput {
	MidiInput* in = nullptr;
	std::string name = "";
};

struct MidiMessageWithInput {
	size_t input = 0;
	MidiMessage msg;
};

struct MidiCubeConfig {
	std::string driver = "UNSPECIFIED";
	unsigned int sample_rate = 48000;
	unsigned int buffer_size = 256;
	unsigned int input_channels = 2;
	ssize_t output_device = 0;
	ssize_t input_device = 0;
	bool screen_sleep = false;
	std::vector<MidiSource> default_sources = {{}};
	MidiControls controls;

	void load(pt::ptree tree);

	pt::ptree save();
};

class MidiCubeWorkstation : public ProgramUser, public MasterPluginHost {
private:
	AudioHandler audio_handler;
	MenuHandler menu_handler;
	MidiCubeConfig config;
	std::function<void(void*, void*)> property_callback;
	std::vector<MidiCubeInput> inputs;
	ControlView* view = nullptr;

	inline void process_midi(MidiMessage& message, size_t input);
public:
	std::array<MidiSource, SOUND_ENGINE_MIDI_CHANNELS> sources;
	size_t used_sources = 1;

	ProgramManager prog_mgr;
	SoundEngineDevice engine;
	PluginManager plugin_mgr;
	Clipboard clipboard;

	std::atomic<bool> updated{false};

	SpinLock lock;

	SpinLock& get_lock();
	MidiBindingHandler* get_binding_handler();
	MenuHandler& get_menu_handler();
	void change_control_view(ControlView *view);
	void set_property_change_callback(
			std::function<void(void*, void*)> cb);
	PluginManager& get_plugin_manager();
	void copy_program();
	bool paste_program();
	void save_program(Program *prog);
	void apply_program(Program *prog);
	void notify_property_update(void* source, void* prop);
	const MidiCubeConfig& get_config() const;
	MidiCubeWorkstation();
	void init();
	inline void process(double& lsample, double& rsample, double* inputs, const size_t input_count, SampleInfo& info);
	std::vector<MidiCubeInput> get_inputs();


	~MidiCubeWorkstation();
};


#endif /* MIDICUBE_MIDICUBE_H_ */
