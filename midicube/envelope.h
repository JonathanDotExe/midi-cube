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
};

enum ADSREnvelopePhase {
	ATTACK, DECAY, SUSTAIN, RELEASE, FINISHED
};

class ADSREnvelope {

public:
	ADSREnvelopePhase phase = FINISHED;
	double volume = 0;

	double amplitude(ADSREnvelopeData& data, double time_step, bool pressed, bool sustain);

	bool is_finished() {
		return phase == FINISHED;
	}

	void reset () {
		phase = ATTACK;
		volume = 0;
	}
};

class EnvelopeFollower {
private:
	std::array<double, ENVELOPE_FOLLOWER_BUFFER_SIZE> buffer = {};
	std::size_t index = 0;
	double highest_signal = 0;
	double highest_time = 0;

	double step_time = 0.05;
public:
	EnvelopeFollower(double step_time = 0.05);
	void apply(double signal, double time, double  time_step);
	double volume();
	void set_step_time(double step_time);
};


#endif /* MIDICUBE_ENVELOPE_H_ */
