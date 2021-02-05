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
#include <string>
#include <array>
#include <functional>

#define SOUND_ENGINE_POLYPHONY 30
#define SOUND_ENGINE_MIDI_CHANNELS 16

class SoundEngineDevice;

struct EngineStatus {
	size_t pressed_notes;
	size_t latest_note_index;
	TriggeredNote* latest_note;
};

class NoteBuffer {
private:
	size_t next_freq_slot(SampleInfo& info);

public:
	std::array<TriggeredNote, SOUND_ENGINE_POLYPHONY> note;

	NoteBuffer();

	void press_note(SampleInfo& info, unsigned int note, double velocity);
	void release_note(SampleInfo& info, unsigned int note, bool invalidate = false);

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

class BaseSoundEngine : public SoundEngine {
private:
	KeyboardEnvironment environment;
	NoteBuffer note;

public:
	std::atomic<unsigned int> sustain_control{64};
	std::atomic<bool> sustain{true};

	void midi_message(MidiMessage& msg, SampleInfo& info);

	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	virtual void process_note_sample(double& lsample, double& rsample, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) = 0;


	virtual void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {

	};

	virtual void control_change(unsigned int control, unsigned int value) {

	};

	virtual bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
		return !note.pressed || (env.sustain && note.release_time >= env.sustain_time);
	};

	virtual ~BaseSoundEngine() {

	};

};


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
	SoundEngine& channel(unsigned int channel) {
		return engines.at(channel);
	}
	std::string get_name() {
		return get_engine_name<T>();
	}
};

enum ArpeggiatorPattern {
	ARP_UP, ARP_DOWN, ARP_RANDOM, ARP_UP_DOWN, ARP_UP_CUSTOM, ARP_DOWN_CUSTOM
};

struct ArpeggiatorPreset {
	ArpeggiatorPattern pattern;
	std::vector<unsigned int> data;
	unsigned int octaves = 1;
	int value = 1;
	bool hold = false;
};

class Arpeggiator {

private:
	unsigned int curr_note = 0;
	std::size_t data_index = 0;
	std::size_t note_index = 0;
	bool restart = true;

public:
	bool on = false;
	ArpeggiatorPreset preset;
	NoteBuffer note;
	Metronome metronome;

	Arpeggiator();

	void apply(SampleInfo& info, std::function<void(SampleInfo&, unsigned int, double)> press, std::function<void(SampleInfo&, unsigned int)> release);

	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

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
	double volume{0.3};
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

	SoundEngine* get_engine(std::vector<SoundEngineBank*> engines, unsigned int channel);

	/**
	 * May only be called from GUI thread after GUI has started
	 */
	void set_engine(ssize_t engine);

	ssize_t get_engine();

	~SoundEngineChannel();

};

enum SoundEngineProperty {
	pEngineMetronomeOn,
	pEngineMetronomeBPM
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

	SoundEngineDevice();

	std::vector<SoundEngineBank*> get_sound_engines();

	void add_sound_engine(SoundEngineBank* engine);

	void send(MidiMessage& message, SampleInfo& info);

	void process_sample(double& lsample, double& rsample, SampleInfo& info);

	void solo (unsigned int channel);

	PropertyValue get(size_t prop, size_t sub_prop = 0);

	void set(size_t prop, PropertyValue value, size_t sub_prop = 0);

	void update_properties();

	~SoundEngineDevice();

};




#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
