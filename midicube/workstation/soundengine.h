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

#include <boost/property_tree/ptree.hpp>

#define CHANNEL_SEQUENCER_AMOUNT 1

#define MOTION_SEQUENCER_AMOUNT 4
#define MOTION_SEQUENCER_LENGTH 16


namespace pt = boost::property_tree;

#define SOUND_ENGINE_SCENE_AMOUNT 8
#define CHANNEL_INSERT_EFFECT_AMOUNT 2
#define SOUND_ENGINE_MASTER_EFFECT_AMOUNT 16

class MidiCube;
class SoundEngineDevice;
class SoundEngineChannel;

struct ChannelSource {
	unsigned int start_note = 0;
	unsigned int end_note = 127;
	unsigned int start_velocity = 0;
	unsigned int end_velocity = 127;
	int octave = 0;
	bool transfer_channel_aftertouch = true;
	bool transfer_pitch_bend = true;
	bool transfer_cc = true;
	bool transfer_prog_change = true;
	bool transfer_other = true;
};

struct SoundEngineScene {
	bool active = false;
	bool sustain = true;
	bool pitch_bend = true;
	ChannelSource source;
};

class SoundEngineChannel : public PluginHost {
private:
	SoundEngineDevice* device = nullptr;

	LocalMidiBindingHandler binder;
	KeyboardEnvironment env;
public:
	std::array<PluginSlot, CHANNEL_SEQUENCER_AMOUNT> sequencers;
	PluginSlot engine;
	BindableTemplateValue<double> volume{0.5, 0, 1};
	BindableTemplateValue<double> panning{0, -1, 1};
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	ssize_t input = 0;

	//Effects
	std::array<PluginSlot, CHANNEL_INSERT_EFFECT_AMOUNT> effects;
	ssize_t master_send = -1;

	size_t polyphony_limit = 0;

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

	void send(const MidiMessage& message, const SampleInfo& info, void* src);

	void process_sample(double& lsample, double& rsample, double* inputs, const size_t input_count, const SampleInfo& info);

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

	SoundEngineDevice* get_device() const {
		return device;
	}


};

struct ChannelProgram {
	bool active = true;
	BindableTemplateValue<double> volume{0.5, 0, 1};
	BindableTemplateValue<double>  panning{0, -1, 1};
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	std::array<PluginSlotProgram, CHANNEL_INSERT_EFFECT_AMOUNT> effects;
	ssize_t send_master = -1;
	size_t polyphony_limit = 0;
	ssize_t input = 0;

	PluginSlotProgram engine_program;
	std::array<PluginSlotProgram, CHANNEL_SEQUENCER_AMOUNT> sequencers;
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
		effect.load(prog.prog, mgr);
	}

	void save(MasterEffectProgram& prog) {
		prog.next_effect = next_effect;
		effect.save(prog.prog);
	}
};

struct Program {
	std::string name;
	unsigned int metronome_bpm = 120;
	std::array<ChannelProgram, SOUND_ENGINE_MIDI_CHANNELS> channels = {{true}};
	std::array<MasterEffectProgram, SOUND_ENGINE_MASTER_EFFECT_AMOUNT> effects;
};

struct MidiSource {
	ssize_t device = 1;
	int channel = 0;
	bool transfer_cc = true;
	bool transfer_pitch_bend = true;
	bool transfer_prog_change = true;
	bool clock_in = false;
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
};


class SoundEngineDevice {

private:
	std::array<MotionSequencer<MOTION_SEQUENCER_LENGTH>, MOTION_SEQUENCER_AMOUNT> motion_sequencers;

	ADSREnvelopeData metronome_env_data{0.0005, 0.02, 0, 0};
	WaveTableADSREnvelope metronome_env;

	size_t clock_beat_count = 0;
	double first_beat_time = 0;

	MidiCube* cube = nullptr;
	SoundEngineDeviceHost host;

public:
	MidiBindingHandler binding_handler;

	Metronome metronome;
	bool play_metronome{false};
	std::array<SoundEngineChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;
	std::array<MasterEffect, SOUND_ENGINE_MASTER_EFFECT_AMOUNT> effects;
	double volume{0.2};
	size_t scene{0};
	std::array<unsigned int, SOUND_ENGINE_SCENE_AMOUNT> scene_ccs = {52, 53, 54, 55, 57, 58, 59, 60};

	unsigned int sustain_control{64};

	SoundEngineDevice();

	void init(MidiCube* cube);

	void send(MidiMessage& message, size_t input, MidiSource& source, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, double* inputs, const size_t input_count, const SampleInfo& info);

	void apply_program(Program* program);

	void save_program(Program* program);

	MidiCube* get_cube() {
		return cube;
	}

	~SoundEngineDevice();

};


#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
