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
#include "filter.h"

struct TriggeredNote {
	double start_time = 0;
	bool pressed = false;
	double release_time = 0;
	unsigned int note = 0;
	double freq = 0;
	double velocity = 0;
	double aftertouch = 0;
	double phase_shift = 0;
	bool valid = false;
};

struct KeyboardEnvironment {
	bool sustain = false;
	double sustain_time = 0;
	double sustain_release_time = 0;
	double pitch_bend = 1;
};

struct ADSREnvelopeData {
	double attack;
	double decay;
	double sustain;
	double release;

	double peak_volume = 1;
	double sustain_time = 0;
	double release_volume = 0;
};

enum ADSREnvelopePhase {
	ATTACK, DECAY, SUSTAIN, RELEASE, FINISHED
};

class LinearADSREnvelope {

public:
	ADSREnvelopePhase phase = FINISHED;
	double last_vol = 0;
	double volume = 0;
	double time = 0;

	double amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain);

	inline bool is_finished() {
		return phase == FINISHED;
	}

	inline void reset () {
		phase = ATTACK;
		volume = 0;
		last_vol = 0;
	}
};

class AnalogADSREnvelope {

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
		phase = ATTACK;
		time = 0;
		last_vol = 0;
		volume = 0;
		last = 0;
	}
};

class EnvelopeFollower {
private:
	Filter filter;
	FilterData data{FilterType::LP_12, 20, 0};
	double value = 0;
public:
	EnvelopeFollower();
	void apply(double signal, double time_step);
	double volume();
};


#endif /* MIDICUBE_ENVELOPE_H_ */
