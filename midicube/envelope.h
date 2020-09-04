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
	double pitch_bend = 0;
};

struct ADSREnvelope {
	double attack;
	double decay;
	double sustain;
	double release;

	double amplitude(double time, bool pressed, double note_start_time, double note_release_time, bool sustain = false, double sustain_time = 0, double sustain_release_time = 0);
	double amplitude(double time, TriggeredNote& note, KeyboardEnvironment& env);
	double is_finished(double time, bool pressed, double note_start_time, double note_release_time, bool sustain = false, double sustain_time = 0, double sustain_release_time = 0);
	double is_finished(double time, TriggeredNote& note, KeyboardEnvironment& env);
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
