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
#include "../device.h"
#include "../synthesis.h"
#include "../metronome.h"
#include <string>
#include <array>
#include <mutex>

#define SOUND_ENGINE_POLYPHONY 30
#define SOUND_ENGINE_MIDI_CHANNELS 16

class SoundEngineDevice;

struct EngineStatus {
	size_t pressed_notes;
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
	virtual void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index) = 0;

	virtual void note_not_pressed(SampleInfo& info, TriggeredNote& note, SoundEngineData& data, size_t note_index) {

	};

	virtual void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& data) {

	};

	virtual void control_change(unsigned int control, unsigned int value, SoundEngineData& data) {

	};

	virtual bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data) {
		return !note.pressed || (env.sustain && note.release_time >= env.sustain_time);
	};

	virtual std::string get_name() = 0;

	virtual SoundEngineData* create_data() {
		return new SoundEngineData();
	}

	virtual ~SoundEngine() {

	};

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

enum class ArpeggiatorPattern {
	UP, DOWN, RANDOM, UP_DOWN, UP_CUSTOM, DOWN_CUSTOM
};

struct ArpeggiatorPreset {
	ArpeggiatorPattern pattern;
	std::vector<unsigned int> data;
	unsigned int octaves = 1;
	int value;
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

	void apply(SampleInfo& info, NoteBuffer& note);

	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

};


class SoundEngineChannel {
private:
	KeyboardEnvironment environment;
	NoteBuffer note;
	SoundEngine* engine = nullptr;
	SoundEngineData* data = nullptr;
	Arpeggiator arp;
	std::mutex engine_mutex;

public:
	std::atomic<double> volume{0.3};
	std::atomic<unsigned int> sustain_control{64};
	std::atomic<bool> sustain{true};

	SoundEngineChannel();

	void send(MidiMessage& message, SampleInfo& info);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	/**
	 * May only be called from GUI thread after GUI has started
	 */
	void set_engine(SoundEngine* engine);

	ssize_t get_engine_index(std::vector<SoundEngine*>& engines);

	SoundEngineData* get_data();

	Arpeggiator& arpeggiator();

	std::string get_engine_name();

	~SoundEngineChannel();

};

class SoundEngineDevice : public AudioDevice {

private:
	std::string identifier;
	std::array<SoundEngineChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;
	std::vector<SoundEngine*> sound_engines;

	ADSREnvelope metronome_env{0.0005, 0.02, 0, 0};

public:
	Metronome metronome;
	std::atomic<bool> play_metronome{true};
	AudioHandler* handler = nullptr; //TODO remove this reference and pass time through send() to be thread-safe

	SoundEngineDevice(std::string identifier);

	std::string get_identifier();

	std::vector<SoundEngine*> get_sound_engines();

	void add_sound_engine(SoundEngine* engine);

	SoundEngineChannel& get_channel(unsigned int channel);

	bool is_audio_input() {
		return true;
	}

	DeviceType type() {
		return DeviceType::SOUND_ENGINE;
	};

	void send(MidiMessage& message);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	~SoundEngineDevice();

};




#endif /* MIDICUBE_SOUNDENGINE_SOUNDENGINE_H_ */
