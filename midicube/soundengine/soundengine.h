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
#include "voice.h"
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

struct EngineStatus {
	size_t pressed_notes = 0;
	size_t latest_note_index = 0;
};

/*
class SoundEngine {

protected:

	SoundEngineChannel* channel = nullptr;
	SoundEngineDevice* device = nullptr;

public:
	virtual void init(SoundEngineChannel* channel);

	virtual bool midi_message(MidiMessage& msg, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit) = 0;

	virtual void press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit) = 0;

	virtual void release_note(SampleInfo& info, unsigned int real_note) = 0;

	virtual EngineStatus process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env) = 0;

	virtual void apply_program(EngineProgram* prog) {

	}

	virtual void save_program(EngineProgram** prog) {

	}

	virtual ~SoundEngine() {

	};

};

template<typename V, size_t P>
class BaseSoundEngine : public SoundEngine {
protected:
	VoiceManager<V, P> note;

public:
	virtual bool midi_message(MidiMessage& msg, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit);

	virtual void press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit);

	virtual void release_note(SampleInfo& info, unsigned int note);

	EngineStatus process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env);

	virtual void process_note_sample(double& lsample, double& rsample, SampleInfo& info, V& note, KeyboardEnvironment& env, size_t note_index) = 0;


	virtual void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {

	};

	virtual bool control_change(unsigned int control, unsigned int value) {
		return false;
	};

	virtual bool note_finished(SampleInfo& info, V& note, KeyboardEnvironment& env, size_t note_index) {
		return !note.pressed || (env.sustain && note.release_time >= env.sustain_time);
	};

	virtual ~BaseSoundEngine() {

	};

};

//BaseSoundEngine
template<typename V, size_t P>
bool BaseSoundEngine<V, P>::midi_message(MidiMessage& message, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit) {
	bool changed = false;
	switch (message.type) {
		case MessageType::NOTE_ON:
			press_note(info, message.note(), message.note() + transpose, message.velocity()/127.0, polyphony_limit);
			break;
		case MessageType::NOTE_OFF:
			release_note(info, message.note());
			break;
		case MessageType::CONTROL_CHANGE:
			changed = control_change(message.control(), message.value());
			break;
		default:
			break;
	}
	return changed;
}

template<typename V, size_t P>
void BaseSoundEngine<V, P>::press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit) {
	this->note.press_note(info, real_note, note, velocity, polyphony_limit);
}

template<typename V, size_t P>
void BaseSoundEngine<V, P>::release_note(SampleInfo& info, unsigned int note) {
	this->note.release_note(info, note);
}

template<typename V, size_t P>
EngineStatus BaseSoundEngine<V, P>::process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env) {
	EngineStatus status = {0, 0};
	//Notes
	for (size_t i = 0; i < P; ++i) {
		if (note.note[i].valid) {
			if (note_finished(info, note.note[i], env, i)) {
				note.note[i].valid = false;
			}
			else {
				++status.pressed_notes; //TODO might cause problems in the future
				note.note[i].phase_shift += (env.pitch_bend - 1) * info.time_step;
				process_note_sample(lsample, rsample, info, note.note[i], env, i);
				if (!status.pressed_notes || note.note[status.latest_note_index].start_time < note.note[i].start_time) {
					status.latest_note_index = i;
				}
			}
		}
	}
	//Static sample
	process_sample(lsample, rsample, info, env, status);

	return status;
}

template <typename T>
std::string get_engine_name();
*/

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
	PluginSlot engine;
	SoundEngineDevice* device = nullptr;

	LocalMidiBindingHandler binder;
public:
	BindableTemplateValue<double> volume{0.5, 0, 1};
	BindableTemplateValue<double> panning{0, -1, 1};
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	Arpeggiator arp;

	//Effects
	std::array<PluginSlot, CHANNEL_INSERT_EFFECT_AMOUNT> effects;
	ssize_t master_send = -1;

	EngineStatus status = {};
	ChannelInfo info;
	size_t polyphony_limit = 0;

	SoundEngineChannel();

	void init_device(SoundEngineDevice* device);

	bool send(MidiMessage& message, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	SoundEngine* get_engine();

	void set_engine(SoundEngine* engine);

	void set_engine_index(ssize_t engine);

	ssize_t get_engine_index();

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
	ssize_t engine_index{-1};
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

	~ChannelProgram() {
		delete engine_program;
	}
};

struct Program {
	std::string name;
	unsigned int metronome_bpm = 120;
	std::array<ChannelProgram, SOUND_ENGINE_MIDI_CHANNELS> channels = {{2, true}};
	std::array<PluginSlotProgram, SOUND_ENGINE_MASTER_EFFECT_AMOUNT> effects;
	std::array<MotionSeqeuncerPreset<MOTION_SEQUENCER_LENGTH>, MOTION_SEQUENCER_AMOUNT> motion_sequencers;
};

/*
enum SoundEngineProperty {
	pEngineMetronomeOn,
	pEngineMetronomeBPM,
	pEngineVolume,
};
*/

class SoundEngineBuilder {
public:
	virtual SoundEngine* build() = 0;

	virtual std::string get_name() = 0;

	virtual bool matches(SoundEngine* engine) = 0;

	virtual ~SoundEngineBuilder() {

	};
};

template <typename T>
class TemplateSoundEngineBuilder : public SoundEngineBuilder {
public:
	inline SoundEngine* build() {
		return new T();
	}
	std::string get_name() {
		return get_engine_name<T>();
	}

	bool matches(SoundEngine* engine) {
		return dynamic_cast<T*>(engine) != nullptr;
	}
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

	std::vector<SoundEngineBuilder*> engine_builders;
	std::vector<EffectBuilder*> effect_builders;

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

	std::vector<SoundEngineBuilder*> get_engine_builders();

	void add_sound_engine(SoundEngineBuilder* engine);

	void add_effect(EffectBuilder* effect);

	bool send(MidiMessage& message, size_t input, MidiSource& source, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	void apply_program(Program* program);

	void save_program(Program* program);

	MidiCube* get_cube() {
		return cube;
	}

	~SoundEngineDevice();

	std::vector<EffectBuilder*> get_effect_builders() {
		return effect_builders;
	}
};


#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
