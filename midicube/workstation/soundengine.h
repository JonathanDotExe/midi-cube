/*
 * soundengine.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_

#include "../framework/core/midi.h"
#include "../framework/dsp/envelope.h"
#include "audio.h"
#include "../framework/dsp/synthesis.h"
#include "../framework/core/metronome.h"
#include "../framework/core/plugin.h"
#include "../framework/util/voice.h"
#include <string>
#include <array>
#include <functional>
#include "../framework/util/clipboard.h"

#include <boost/property_tree/ptree.hpp>

#define CHANNEL_SEQUENCER_AMOUNT 1

#define MOTION_SEQUENCER_AMOUNT 4
#define MOTION_SEQUENCER_LENGTH 16


namespace pt = boost::property_tree;

#define SOUND_ENGINE_SCENE_AMOUNT 8
#define CHANNEL_INSERT_EFFECT_AMOUNT 2
#define SOUND_ENGINE_MASTER_EFFECT_AMOUNT 16

class MidiCubeWorkstation;
class SoundEngineDevice;
class SoundEngineChannel;

struct ChannelSource {
	unsigned int start_note = 0;
	unsigned int end_note = 127;
	unsigned int start_velocity = 0;
	unsigned int end_velocity = 127;
	int octave = 0;
	int update_channel = 0;
};

struct ChannelRedirect {
	int redirect_to = -1;
	int channel = -1;
};

struct SoundEngineScene {
	bool active = false;
	bool sustain = true;
	bool pitch_bend = true;
	ChannelSource source;

	bool is_default() {
		return active == false &&
				sustain == true &&
				pitch_bend == true &&
				source.start_note == 0 &&
				source.end_note == 127 &&
				source.start_velocity == 0 &&
				source.end_velocity == 127 &&
				source.octave == 0 &&
				source.update_channel == 0;
	}
};

/*
struct ChannelProgram : public Copyable {
	bool active = true;
	BindableTemplateValue<double> volume{0.5, 0, 1};
	BindableTemplateValue<double>  panning{0, -1, 1};
	ChannelRedirect redirect;
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	std::array<PluginSlotProgram, CHANNEL_INSERT_EFFECT_AMOUNT> effects;
	ssize_t send_master = -1;
	size_t polyphony_limit = 0;
	ssize_t input = 0;

	PluginSlotProgram engine_program;
	std::array<PluginSlotProgram, CHANNEL_SEQUENCER_AMOUNT> sequencers;

	ChannelProgram() : Copyable() {

	}

	bool is_default() {
		return engine_program.get_program() == nullptr && redirect.channel == -1 && redirect.redirect_to == -1;
	}
};*/

class SoundEngineChannel : public PluginHost, public ControlHost {
private:
	SoundEngineDevice* device = nullptr;

	LocalMidiBindingHandler binder;
	KeyboardEnvironment env;
public:
	std::array<PluginSlot, CHANNEL_SEQUENCER_AMOUNT> sequencers;
	PluginSlot engine;
	BindableTemplateValue<double> volume{0.5, 0, 1};
	BindableTemplateValue<double> panning{0, -1, 1};
	ChannelRedirect redirect;
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	ssize_t input = 0;

	//Effects
	std::array<PluginSlot, CHANNEL_INSERT_EFFECT_AMOUNT> effects;
	ssize_t master_send = -1;

	size_t polyphony_limit = 0;

	MasterPluginHost& get_master_host();
	void notify_property_update(void *property);
	int get_transpose();
	void notify_property_update(void *source, void *prop);
	virtual SpinLock& get_lock();
	void recieve_midi(const MidiMessage &message,
			const SampleInfo &info, void* src);
	MidiBindingHandler* get_binding_handler();
	Plugin* get_plugin(std::string identifier);
	const Metronome& get_metronome();
	const KeyboardEnvironment& get_environment();
	SoundEngineChannel();

	void init_device(SoundEngineDevice* device);

	inline void send(const MidiMessage& message, const SampleInfo& info, void* src);

	inline void process_sample(double& lsample, double& rsample, double* inputs, const size_t input_count, const SampleInfo& info);

	inline bool send_midi(MessageType type);

	PluginInstance* get_engine();

	void set_engine(Plugin* engine);

	~SoundEngineChannel();

	unsigned int get_end_note() const;

	void set_end_note(unsigned int endNote = 127);

	unsigned int get_end_velocity() const;

	void set_end_velocity(unsigned int endVelocity = 127);

	int get_octave() const;

	void set_octave(int octave = 0);

	unsigned int get_start_note() const;

	void set_start_note(unsigned int startNote = 0);

	int get_update_channel() const;

	void set_update_channel(int update_channel = 0);

	unsigned int get_start_velocity() const;

	void set_start_velocity(unsigned int startVelocity = 0);

	bool is_transfer_cc() const;

	void set_transfer_cc(bool transferCc = true);

	bool is_transfer_channel_aftertouch() const;

	void set_transfer_channel_aftertouch(
			bool transferChannelAftertouch = true);

	bool is_transfer_other() const;

	void set_transfer_other(bool transferOther = true);

	bool is_transfer_pitch_bend() const;

	void set_transfer_pitch_bend(bool transferPitchBend = true);

	bool is_transfer_prog_change() const;

	void set_transfer_prog_change(bool transferProgChange = true);

	bool is_sustain() const;

	void set_sustain(bool sustain = true);

	bool is_pitch_bend() const;

	void set_pitch_bend(bool pitch_bend = true);

	bool is_active() const;

	void set_active(bool active = false);

	void copy_channel();

	bool paste_channel();

	SoundEngineDevice* get_device() const {
		return device;
	}

	void apply_program(ChannelProgram* program, PluginManager* mgr);

	void save_program(ChannelProgram* program);

	const MidiControls& get_controls();

};

struct MasterEffectProgram {
	ssize_t next_effect = -1;
	PluginSlotProgram prog;
};

struct MasterEffect {
public:
	PluginSlot effect;
	double lsample = 0;
	double rsample = 0;
	size_t next_effect = 0;

	void load(MasterEffectProgram& prog, PluginManager* mgr) {
		next_effect = prog.next_effect;
		lsample = 0;
		rsample = 0;
		effect.load(prog.prog, mgr);
	}

	void save(MasterEffectProgram& prog) {
		prog.next_effect = next_effect;
		effect.save(prog.prog);
	}
};

struct Program : public Copyable {
	std::string name;
	std::string host;
	unsigned int version = 0;
	pt::ptree data;

	Program(std::string name) : Copyable() {
		this->name = name;
	}

};

struct MidiSource {
	size_t device = 1;
	int channel = 0;
};

class SoundEngineDevice;

class SoundEngineDeviceHost : public PluginHost {
private:
	KeyboardEnvironment env;
	SoundEngineDevice* device = nullptr;
public:
	SoundEngineDeviceHost() : PluginHost() {

	}

	void init(SoundEngineDevice* device);
	void recieve_midi(const MidiMessage &message,
			const SampleInfo &info, void* src);
	Plugin* get_plugin(std::string identifier);
	MidiBindingHandler* get_binding_handler();
	const Metronome& get_metronome();
	const KeyboardEnvironment& get_environment();
	SpinLock& get_lock();
	int get_transpose();


	~SoundEngineDeviceHost() {

	}

	void notify_property_update(void *source, void *prop);
	const MidiControls& get_controls();
	MasterPluginHost& get_master_host();
};


class SoundEngineDevice {

private:
	std::array<MotionSequencer<MOTION_SEQUENCER_LENGTH>, MOTION_SEQUENCER_AMOUNT> motion_sequencers;

	ADSREnvelopeData metronome_env_data{0.0005, 0.02, 0, 0};
	WaveTableADSREnvelope metronome_env;

	size_t clock_beat_count = 0;
	double first_beat_time = 0;

	MidiCubeWorkstation* cube = nullptr;
	SoundEngineDeviceHost host;

public:
	MidiBindingHandler binding_handler;

	Metronome metronome;
	bool play_metronome{false};
	std::array<SoundEngineChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;
	std::array<MasterEffect, SOUND_ENGINE_MASTER_EFFECT_AMOUNT> effects;
	double volume{0.2};
	size_t scene{0};

	SoundEngineDevice();

	void init(MidiCubeWorkstation* cube);

	void send(const MidiMessage& message, size_t input, const SampleInfo& info);

	void process_sample(double& lsample, double& rsample, double* inputs, const size_t input_count, const SampleInfo& info);

	void apply_program(Program* program);

	void save_program(Program* program);

	void clock_sync(const MidiMessage& message, const SampleInfo& info);

	MidiCubeWorkstation* get_cube() {
		return cube;
	}

	~SoundEngineDevice();

};


#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
