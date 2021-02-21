/*
 * soundengine.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_

#include "../midi.h"
#include "../program.h"
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

class SoundEngineDevice;

struct EngineStatus {
	size_t pressed_notes;
	size_t latest_note_index;
};

class SoundEngineData {
public:
	virtual SoundEngineData* copy() {
		return new SoundEngineData();
	}

	virtual ~SoundEngineData() {

	}
};

class SoundEngine {

public:
	virtual void midi_message(MidiMessage& msg, SampleInfo& info) = 0;

	virtual void press_note(SampleInfo& info, unsigned int note, double velocity) = 0;

	virtual void release_note(SampleInfo& info, unsigned int note) = 0;

	virtual void process_sample(double& lsample, double& rsample, SampleInfo& info) = 0;

	virtual ~SoundEngine() {

	};

};

template<typename V, size_t P>
class BaseSoundEngine : public SoundEngine {
private:
	KeyboardEnvironment environment;

protected:
	VoiceManager<V, P> note;

public:
	std::atomic<unsigned int> sustain_control{64};
	std::atomic<bool> sustain{true};


	void midi_message(MidiMessage& msg, SampleInfo& info);

	virtual void press_note(SampleInfo& info, unsigned int note, double velocity);

	virtual void release_note(SampleInfo& info, unsigned int note);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

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
void BaseSoundEngine<V, P>::process_sample(double& lsample, double& rsample, SampleInfo& info) {
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
	ssize_t input = -1;
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

class SoundEngineChannel : public PropertyHolder {
private:
	std::atomic<ssize_t> engine_index{0};

public:
	double volume{1};
	bool active{false};
	double panning = 0;
	ChannelSource source;
	Arpeggiator arp;
	Looper looper;

	//Effects
	VocoderPreset vocoder_preset;
	VocoderEffect vocoder;
	BitCrusherPreset bitcrusher_preset;
	BitCrusherEffect bitcrusher;

	SoundEngineChannel();

	void send(MidiMessage& message, SampleInfo& info, SoundEngine& engine);

	void process_sample(double& lsample, double& rsample, SampleInfo& info, Metronome& metronome, SoundEngine* engine);

	PropertyValue get(size_t prop, size_t sub_prop);

	void set(size_t prop, PropertyValue value, size_t sub_prop);

	void update_properties();

	inline SoundEngine* get_engine(std::vector<SoundEngineBank*>& engines, unsigned int channel);

	/**
	 * May only be called from GUI thread after GUI has started
	 */
	void set_engine(ssize_t engine);

	ssize_t get_engine();

	~SoundEngineChannel();

};

enum SoundEngineProperty {
	pEngineMetronomeOn,
	pEngineMetronomeBPM,
	pEngineVolume,
};

class SoundEngineDevice : public PropertyHolder {

private:
	std::vector<SoundEngineBank*> sound_engines;

	ADSREnvelopeData metronome_env_data{0.0005, 0.02, 0, 0};
	ADSREnvelope metronome_env;

public:
	Metronome metronome;
	std::atomic<bool> play_metronome{false};
	std::array<SoundEngineChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;
	double volume{0.2};

	SoundEngineDevice();

	std::vector<SoundEngineBank*> get_sound_engines();

	void add_sound_engine(SoundEngineBank* engine);

	void send(MidiMessage& message, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	void solo (unsigned int channel);

	PropertyValue get(size_t prop, size_t sub_prop = 0);

	void set(size_t prop, PropertyValue value, size_t sub_prop = 0);

	void update_properties();

	void apply_program(std::array<ChannelProgram*, SOUND_ENGINE_MIDI_CHANNELS>& channels);

	void save_program(std::array<ChannelProgram*, SOUND_ENGINE_MIDI_CHANNELS>& channels);

	~SoundEngineDevice();

};




#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
