/*
 * envelope.h
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_ENVELOPE_H_
#define MIDICUBE_ENVELOPE_H_

#define ENVELOPE_FOLLOWER_BUFFER_SIZE 44100

#include <array>
#include "../core/audio.h"
#include "../core/midi.h"
#include "filter.h"
#include "wavetable.h"
#include "../core/metronome.h"

struct TriggeredNote {
	double start_time = 0;
	bool pressed = false;
	double release_time = 0;
	unsigned int real_note = 0;
	unsigned int note = 0;
	double freq = 0;
	double velocity = 0;
	double aftertouch = 0;
	bool valid = false;
	unsigned int channel = 0;
};

struct KeyboardEnvironment {
	bool sustain = false;
	double sustain_time = 0;
	double sustain_release_time = 0;
	double pitch_bend = 1;
	double aftertouch = 0;
	double pitch_bend_percent = 0.5;
};

enum ADSREnvelopeShape {
	LINEAR_ADSR,
	ANALOG_ADSR,
	EXPONENTIAL_ADSR
};

struct ADSREnvelopeData {
	double attack = 0;
	double decay = 0;
	double sustain = 1;
	double release = 0;

	ADSREnvelopeShape attack_shape = ADSREnvelopeShape::LINEAR_ADSR;
	ADSREnvelopeShape pre_decay_shape = ADSREnvelopeShape::ANALOG_ADSR;
	ADSREnvelopeShape decay_shape = ADSREnvelopeShape::ANALOG_ADSR;
	ADSREnvelopeShape release_shape = ADSREnvelopeShape::ANALOG_ADSR;

	double hold = 0;
	double pre_decay = 0;

	double attack_hold = 0;
	double peak_volume = 1;
	double decay_volume = 1;
	double sustain_time = 0;
	double release_volume = 0;

	bool pedal_catch = false;
};

enum ADSREnvelopePhase {
	HOLD, ATTACK, ATTACK_HOLD, PRE_DECAY, DECAY, SUSTAIN, RELEASE, FINISHED
};

class WaveTableADSREnvelope {

public:
	ADSREnvelopePhase phase = FINISHED;
	double time = 0;
	double last_vol = 0;
	double volume = 0;
	double last = 0;

	double amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain);

	inline bool is_finished() {
		return phase == FINISHED;
	}

	inline void reset () {
		phase = HOLD;
		time = 0;
		last_vol = 0;
		volume = 0;
		last = 0;
	}
};

struct MotionSeqeuncerEntry {
	unsigned int beats = 1;
	double value = 0;
	ADSREnvelopeShape shape = ADSREnvelopeShape::LINEAR_ADSR;
};

template<size_t N>
struct MotionSequncerPreset {
	unsigned int clock_value = 1;
	std::array<MotionSeqeuncerEntry, N> entries;
};

template<size_t N>
class MotionSequencer {

public:

	double amplitude(MotionSequncerPreset<N>& preset, Metronome& metronome, const SampleInfo& info);

};

class EnvelopeFollower {
private:
	Filter filter;
	FilterData data{FilterType::LP_12, 20, 0};
	double value = 0;
public:
	EnvelopeFollower();
	void apply(double signal, double time_step);
	inline double volume();
};

class NoiseGate {
private:
	EnvelopeFollower follower;
	PortamendoBuffer volume{0};

public:
	double attack = 0.05;
	double release = 0.05;
	double gate = 0.001;

	double apply(double signal, double time_step);

};

template<size_t N>
double MotionSequencer<N>::amplitude(MotionSequncerPreset<N> &preset,
		Metronome &metronome, const SampleInfo& info) {
	//Find beat
	double value = preset.clock_value;
	if (preset.clock_value <= 0) {
		value = 1.0/(-fmin(preset.clock_value, -1));
	}
	double beat = metronome.beats(info.time, info.sample_rate, value);
	//Count beats
	unsigned int count = 0;
	for (size_t i = 0; i < N; ++i) {
		count++;
	}

	beat = fmod(beat, count);
	unsigned int curr_beat = 0;
	double vol = 0;
	//Value
	for (size_t i = 0; i < N; ++i) {
		//Current beat
		if (curr_beat >= (unsigned int) beat) {
			size_t next_index = (i + 1) % N;
			double prog = (beat - curr_beat)/preset.entries[next_index].beats;
			vol = (1 - prog) * preset.entries[i].value + preset.entries[i].value * prog; //TODO shape
			break;
		}
		curr_beat += preset.entries[i].beats;
	}
	return vol;
}

#endif /* MIDICUBE_ENVELOPE_H_ */
