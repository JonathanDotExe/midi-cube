/*
 * soundengine.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_

#include "../midi.h"
#include "../envelope.h"
#include "../audio.h"
#include "../synthesis.h"
#include "../metronome.h"
#include "../looper.h"
#include "../effect/vocoder.h"
#include "../effect/bitcrusher.h"
#include "../property.h"
#include "voice.h"
#include <string>
#include <array>
#include <functional>

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

#define SOUND_ENGINE_SCENE_AMOUNT 8

class SoundEngineDevice;

struct EngineStatus {
	size_t pressed_notes = 0;
	size_t latest_note_index = 0;
};

class EngineProgram {
public:
	virtual void load(pt::ptree tree) = 0;

	virtual pt::ptree save() = 0;

	virtual ~EngineProgram() {

	}
};

class SoundEngine {

public:
	virtual void midi_message(MidiMessage& msg, SampleInfo& info) = 0;

	virtual void press_note(SampleInfo& info, unsigned int note, double velocity) = 0;

	virtual void release_note(SampleInfo& info, unsigned int note) = 0;

	virtual EngineStatus process_sample(double& lsample, double& rsample, SampleInfo& info) = 0;

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
	KeyboardEnvironment environment;

public:
	std::atomic<unsigned int> sustain_control{64};
	std::atomic<bool> sustain{true};


	void midi_message(MidiMessage& msg, SampleInfo& info);

	virtual void press_note(SampleInfo& info, unsigned int note, double velocity);

	virtual void release_note(SampleInfo& info, unsigned int note);

	EngineStatus process_sample(double& lsample, double& rsample, SampleInfo& info);

	virtual void process_note_sample(double& lsample, double& rsample, SampleInfo& info, V& note, KeyboardEnvironment& env, size_t note_index) = 0;


	virtual void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {

	};

	virtual void control_change(unsigned int control, unsigned int value) {

	};

	virtual bool note_finished(SampleInfo& info, V& note, KeyboardEnvironment& env, size_t note_index) {
		return !note.pressed || (env.sustain && note.release_time >= env.sustain_time);
	};

	virtual ~BaseSoundEngine() {

	};

};

//BaseSoundEngine
template<typename V, size_t P>
void BaseSoundEngine<V, P>::midi_message(MidiMessage& message, SampleInfo& info) {
	double pitch;
	switch (message.type) {
		case MessageType::NOTE_ON:
			press_note(info, message.note(), message.velocity()/127.0);
			break;
		case MessageType::NOTE_OFF:
			release_note(info, message.note());
			break;
		case MessageType::CONTROL_CHANGE:
			control_change(message.control(), message.value());
			//Sustain
			if (message.control() == sustain_control) {
				bool new_sustain = message.value() != 0;
				if (new_sustain != environment.sustain) {
					if (new_sustain) {
						environment.sustain_time = info.time;
					}
					else {
						environment.sustain_release_time = info.time;
					}
					environment.sustain = new_sustain;
				}
			}
			break;
		case MessageType::PITCH_BEND:
			pitch = (message.get_pitch_bend()/8192.0 - 1.0) * 2;
			environment.pitch_bend = note_to_freq_transpose(pitch);
			break;
		default:
			break;
	}
}

template<typename V, size_t P>
void BaseSoundEngine<V, P>::press_note(SampleInfo& info, unsigned int note, double velocity) {
	this->note.press_note(info, note, velocity);
}

template<typename V, size_t P>
void BaseSoundEngine<V, P>::release_note(SampleInfo& info, unsigned int note) {
	this->note.release_note(info, note);
}

template<typename V, size_t P>
EngineStatus BaseSoundEngine<V, P>::process_sample(double& lsample, double& rsample, SampleInfo& info) {
	EngineStatus status = {0, 0};
	//Notes
	for (size_t i = 0; i < P; ++i) {
		if (note.note[i].valid) {
			if (note_finished(info, note.note[i], environment, i)) {
				note.note[i].valid = false;
			}
			else {
				++status.pressed_notes; //TODO might cause problems in the future
				note.note[i].phase_shift += (environment.pitch_bend - 1) * info.time_step;
				process_note_sample(lsample, rsample, info, note.note[i], environment, i);
				if (!status.pressed_notes || note.note[status.latest_note_index].start_time < note.note[i].start_time) {
					status.latest_note_index = i;
				}
			}
		}
	}
	//Static sample
	process_sample(lsample, rsample, info, environment, status);

	return status;
}


class SoundEngineBank {
public:
	virtual SoundEngine& channel(unsigned int channel) = 0;

	virtual std::string get_name() = 0;

	virtual ~SoundEngineBank() {

	};
};

template <typename T>
std::string get_engine_name();

template <typename T>
class TemplateSoundEngineBank : public SoundEngineBank {
private:
	std::array<T, SOUND_ENGINE_MIDI_CHANNELS> engines;

public:
	inline SoundEngine& channel(unsigned int channel) {
		return engines[channel];
	}
	std::string get_name() {
		return get_engine_name<T>();
	}
};

struct ChannelSource {
	ssize_t input = 1;
	unsigned int channel = 0;
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

enum SoundEngineChannelProperty {
	pChannelActive,
	pChannelVolume,
	pChannelPanning,
	pChannelSoundEngine,

	pChannelInputDevice,
	pChannelInputChannel,
	pChannelStartNote,
	pChannelEndNote,
	pChannelStartVelocity,
	pChannelEndVelocity,
	pChannelOctave,
	pChannelTransferChannelAftertouch,
	pChannelTransferPitchBend,
	pChannelTransferCC,
	pChannelTransferProgChange,
	pChannelTransferOther,

	pArpeggiatorOn,
	pArpeggiatorPattern,
	pArpeggiatorOctaves,
	pArpeggiatorStep,
	pArpeggiatorHold,
	pArpeggiatorBPM
};

struct SoundEngineScene {
	bool active = false;
	ChannelSource source;
};

class SoundEngineDevice;

class SoundEngineChannel : public PropertyHolder {
private:
	SoundEngine* engine = nullptr;
	SoundEngineDevice* device = nullptr;

public:
	double volume = 0.5;
	double panning = 0;
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	Arpeggiator arp;
	Looper looper;

	//Effects
	VocoderPreset vocoder_preset;
	VocoderEffect vocoder;
	BitCrusherPreset bitcrusher_preset;
	BitCrusherEffect bitcrusher;

	EngineStatus status = {};

	SoundEngineChannel();

	void init_device(SoundEngineDevice* device) {
		if (!this->device) {
			this->device = device;
		}
	}

	void send(MidiMessage& message, SampleInfo& info, size_t scene);

	void process_sample(double& lsample, double& rsample, SampleInfo& info, Metronome& metronome, size_t scene);

	PropertyValue get(size_t prop, size_t sub_prop);

	void set(size_t prop, PropertyValue value, size_t sub_prop);

	void update_properties();

	inline SoundEngine* get_engine();

	inline void set_engine(SoundEngine* engine);

	void set_engine_index(ssize_t engine);

	ssize_t get_engine_index();

	~SoundEngineChannel();

};

struct ChannelProgram {
	ssize_t engine_index{-1};
	double volume = 0.5;
	double panning = 0;
	std::array<SoundEngineScene, SOUND_ENGINE_SCENE_AMOUNT> scenes;

	unsigned int arpeggiator_bpm = 120;
	bool arp_on;
	ArpeggiatorPreset arpeggiator;

	EngineProgram* engine_program = nullptr;
};

struct Program {
	std::string name;
	unsigned int metronome_bpm = 120;
	std::array<ChannelProgram, SOUND_ENGINE_MIDI_CHANNELS> channels = {{2, true}};
};

enum SoundEngineProperty {
	pEngineMetronomeOn,
	pEngineMetronomeBPM,
	pEngineVolume,
};


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

class SoundEngineDevice : public PropertyHolder {

private:
	std::vector<SoundEngineBuilder*> engine_builders;

	ADSREnvelopeData metronome_env_data{0.0005, 0.02, 0, 0};
	LinearADSREnvelope metronome_env;

public:
	Metronome metronome;
	std::atomic<bool> play_metronome{false};
	std::array<SoundEngineChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;
	double volume{0.2};
	std::atomic<size_t> scene{0};

	SoundEngineDevice();

	std::vector<SoundEngineBuilder*> get_engine_builders();

	void add_sound_engine(SoundEngineBuilder* engine);

	void send(MidiMessage& message, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	PropertyValue get(size_t prop, size_t sub_prop = 0);

	void set(size_t prop, PropertyValue value, size_t sub_prop = 0);

	void update_properties();

	void apply_program(Program* program);

	void save_program(Program* program);

	~SoundEngineDevice();

};


#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
