/*
 * soundengine.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_

#include "../framework/core/midi.h"
#include "../effect/effect.h"
#include "../framework/dsp/envelope.h"
#include "../audio.h"
#include "../framework/dsp/synthesis.h"
#include "../looper.h"
#include "../framework/core/metronome.h"
#include "../framework/core/plugin.h"
#include "../framework/util/voice.h"
#include <string>
#include <array>
#include <functional>

#include <boost/property_tree/ptree.hpp>

#define MOTION_SEQUENCER_AMOUNT 4
#define MOTION_SEQUENCER_LENGTH 16


namespace pt = boost::property_tree;

#define SOUND_ENGINE_SCENE_AMOUNT 8
#define CHANNEL_INSERT_EFFECT_AMOUNT 2
#define SOUND_ENGINE_MASTER_EFFECT_AMOUNT 16

class MidiCube;
class SoundEngineDevice;
class SoundEngineChannel;

struct ChannelInfo {
	double aftertouch = 0;
};

struct ChannelSource {
	ssize_t input = 1;
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

class SoundEngineChannel {
private:
	SoundEngineDevice* device = nullptr;

	LocalMidiBindingHandler binder;
public:
	PluginSlot engine;
	BindableTemplateValue<double> volume{0.5, 0, 1};
	BindableTemplateValue<double> panning{0, -1, 1};
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	Arpeggiator arp;

	//Effects
	std::array<PluginSlot, CHANNEL_INSERT_EFFECT_AMOUNT> effects;
	ssize_t master_send = -1;

	ChannelInfo info;
	size_t polyphony_limit = 0;

	SoundEngineChannel();

	void init_device(SoundEngineDevice* device);

	bool send(MidiMessage& message, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	PluginInstance* get_engine();

	void set_engine(Plugin* engine);

	~SoundEngineChannel();

	unsigned int get_end_note() const;

	void set_end_note(unsigned int endNote = 127);

	unsigned int get_end_velocity() const;

	void set_end_velocity(unsigned int endVelocity = 127);

	ssize_t get_input() const;

	void set_input(ssize_t input = 1);

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

	unsigned int arpeggiator_bpm = 120;
	bool arp_on;
	ArpeggiatorPreset arpeggiator;

	PluginSlotProgram engine_program;
};

struct Program {
	std::string name;
	unsigned int metronome_bpm = 120;
	std::array<ChannelProgram, SOUND_ENGINE_MIDI_CHANNELS> channels = {{true}};
	std::array<PluginSlotProgram, SOUND_ENGINE_MASTER_EFFECT_AMOUNT> effects;
	std::array<MotionSeqeuncerPreset<MOTION_SEQUENCER_LENGTH>, MOTION_SEQUENCER_AMOUNT> motion_sequencers;
};

struct MidiSource {
	ssize_t device = 1;
	int channel = 0;
	bool transfer_cc = true;
	bool transfer_pitch_bend = true;
	bool transfer_prog_change = true;
	bool clock_in = false;
};


class SoundEngineDevice {

private:
	std::array<MotionSequencer<MOTION_SEQUENCER_LENGTH>, MOTION_SEQUENCER_AMOUNT> motion_sequencers;

	ADSREnvelopeData metronome_env_data{0.0005, 0.02, 0, 0};
	WaveTableADSREnvelope metronome_env;

	size_t clock_beat_count = 0;
	double first_beat_time = 0;

	MidiCube* cube = nullptr;

public:
	MidiBindingHandler binding_handler;
	KeyboardEnvironment env;

	Metronome metronome;
	Looper looper;
	bool play_metronome{false};
	std::array<SoundEngineChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;
	std::array<PluginSlot, SOUND_ENGINE_MASTER_EFFECT_AMOUNT> effects;
	double volume{0.2};
	size_t scene{0};
	std::array<unsigned int, SOUND_ENGINE_SCENE_AMOUNT> scene_ccs = {52, 53, 54, 55, 57, 58, 59, 60};

	size_t motion_sequencer_amount = 0;
	std::array<MotionSeqeuncerPreset<MOTION_SEQUENCER_LENGTH>, MOTION_SEQUENCER_AMOUNT> motion_sequencer_presets;
	std::array<double, MOTION_SEQUENCER_AMOUNT> motion_sequencer_values = {};

	unsigned int sustain_control{64};

	SoundEngineDevice();

	void init(MidiCube* cube);

	bool send(MidiMessage& message, size_t input, MidiSource& source, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	void apply_program(Program* program);

	void save_program(Program* program);

	MidiCube* get_cube() {
		return cube;
	}

	~SoundEngineDevice();

};


#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
