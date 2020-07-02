/*
 * soundengine.h
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_H_

#include "device.h"
#include "midi.h"
#include "synthesis.h"
#include "envelope.h"
#include <string>
#include <array>

#define SOUND_ENGINE_POLYPHONY 30


class SoundEngine {

public:
	virtual void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, double pitch_time) = 0;

	virtual void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {

	};

	virtual void control_change(unsigned int control, unsigned int value) {

	};

	virtual bool note_finished(TriggeredNote& note, double time) {
		return !note.pressed;
	}

	virtual std::string get_name() = 0;

	virtual ~SoundEngine() {

	}

};

#define SYNTH_VIBRATO_RATE 6
#define SYNTH_VIBRATO_DETUNE 0.25

class PresetSynth : public SoundEngine {

private:
	double detune;
	double ndetune;
	double vib_detune;
	double vibrato;
	ADSREnvelope env{0.0005, 0.0, 1, 0.0005};
public:

	PresetSynth();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, double phase_mul);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(TriggeredNote& note, double time);

	std::string get_name();

};

#define ORGAN_DRAWBAR_COUNT 9
#define ROTARY_CUTOFF 800
#define ROTARY_HORN_SLOW_FREQUENCY 0.8
#define ROTARY_HORN_FAST_FREQUENCY 6.8
#define ROTARY_BASS_SLOW_FREQUENCY 0.76
#define ROTARY_BASS_FAST_FREQUENCY 6.5
#define SPEAKER_RADIUS 0.1
#define HORN_RADIUS 0.15
#define BASS_RADIUS 0.15
#define SOUND_SPEED 343.2
#define ORGAN_REP_DELAY 0.000728226

struct B3OrganData {
	std::array<int, ORGAN_DRAWBAR_COUNT> drawbars = {8, 8, 8, 8, 8, 8, 8, 8, 8};
	std::array<unsigned int, ORGAN_DRAWBAR_COUNT> drawbar_ccs = {1, 2, 3, 4, 5, 6, 7, 8, 8};
	bool rotary = false;
	bool rotary_fast = false;
	unsigned int rotary_cc = 9;
	unsigned int rotary_speed_cc = 10;
};

class B3Organ : public SoundEngine {

private:
	B3OrganData data;
	std::array<double, ORGAN_DRAWBAR_COUNT> drawbar_harmonics;
	DelayBuffer left_horn_del;
	DelayBuffer left_bass_del;
	DelayBuffer right_horn_del;
	DelayBuffer right_bass_del;
public:
	B3Organ();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, double phase_mul);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	void control_change(unsigned int control, unsigned int value);

	std::string get_name();

};

class SoundEngineDevice : public AudioDevice {

private:
	std::string identifier;
	std::array<TriggeredNote, SOUND_ENGINE_POLYPHONY> note;
	double pitch_bend = 1;
	double phase_multiplier = 0;
	SoundEngine* engine;

	size_t next_freq_slot();

public:

	AudioHandler* handler = nullptr; //TODO remove this reference and pass time through send() to be thread-safe

	SoundEngineDevice(std::string identifier);

	std::string get_identifier();

	bool is_audio_input() {
		return true;
	}

	void send(MidiMessage& message);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	~SoundEngineDevice();

};




#endif /* MIDICUBE_SOUNDENGINE_H_ */
