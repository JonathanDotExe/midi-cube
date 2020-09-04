/*
 * envelope.cpp
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#include "envelope.h"
#include <cmath>
#include <iostream>

//ADSREnvelope
double ADSREnvelope::amplitude(double time, bool pressed, double note_start_time, double note_release_time, bool sustain, double sustain_time, double sustain_release_time) {
	double release_time = fmax(note_release_time, sustain_release_time);
	if (pressed || (sustain && sustain_time <= note_release_time)) {
		//Attack
		if (time <= note_start_time + attack) {
			return (time - note_start_time)/attack;
		}
		//Decay
		else if (time <= note_start_time + attack + decay) {
			return 1 - (time - note_start_time - attack)/decay * (1 - this->sustain);
		}
		//Sustain
		else {
			return this->sustain;
		}
	}
	else if (time - release_time > release) {
		return 0;
	}
	else {
		double last_vol = this->sustain;
		double held_time = release_time - note_start_time;
		if (held_time <= attack) {
			last_vol = held_time/attack;
		}
		else if (held_time < attack + decay) {
			last_vol = 1 - (held_time - attack)/decay * (1 - this->sustain);
		}
		return last_vol - (time - release_time)/release * last_vol;
	}

	return 0;

/*
 * 	double release_time = fmax(note.release_time, env.sustain_release_time);
	if (note.pressed || (env.sustain && env.sustain_time <= note.release_time)) {
		//Attack
		if (time <= note.start_time + attack) {
			return (time - note.start_time)/attack;
		}
		//Decay
		else if (time <= note.start_time + attack + decay) {
			return 1 - (time - note.start_time - attack)/decay * (1 - sustain);
		}
		//Sustain
		else {
			return sustain;
		}
	}
	else if (time - release_time > release) {
		return 0;
	}
	else {
		double last_vol = sustain;
		double held_time = release_time - note.start_time;
		if (held_time <= attack) {
			last_vol = held_time/attack;
		}
		else if (held_time < attack + decay) {
			last_vol = 1 - (held_time - attack)/decay * (1 - sustain);
		}
		return last_vol - (time - release_time)/release * last_vol;
	}

	return 0;
 */
}

double ADSREnvelope::is_finished(double time, bool pressed, double note_start_time, double note_release_time, bool sustain, double sustain_time, double sustain_release_time) {
	double release_time = fmax(note_release_time, sustain_release_time);
	return !pressed && (!sustain || sustain_time > note_release_time) && time - release_time > release;
}

double ADSREnvelope::amplitude(double time, TriggeredNote& note, KeyboardEnvironment& env) {
	return amplitude(time, note.pressed, note.start_time, note.release_time, env.sustain, env.sustain_time, env.sustain_release_time);
}

double ADSREnvelope::is_finished(double time, TriggeredNote& note, KeyboardEnvironment& env) {
	return is_finished(time, note.pressed, note.start_time, note.release_time, env.sustain, env.sustain_time, env.sustain_release_time);
}


//EnvelopeFollower
EnvelopeFollower::EnvelopeFollower(double step_time) {
	this->step_time = step_time;
}

void EnvelopeFollower::apply(double signal, double time, double time_step) {
	buffer[index] = signal;
	if (std::abs(signal) >= highest_signal) {
		highest_signal = std::abs(signal);
		highest_time = time;
	}
	else if (highest_time < time - step_time) {
		highest_signal = -1;
		//Look for new highest signal
		size_t timespan = round(step_time / time_step);
		for (size_t i = 0; i < timespan; ++i) {
			size_t pos = (index - i)%buffer.size();
			if (std::abs(buffer[pos]) >= highest_signal) {
				highest_signal = std::abs(buffer[pos]);
				highest_time = time - time_step * i;
			}
		}
	}

	index = (index + 1) % buffer.size();
}

double EnvelopeFollower::volume() {
	return highest_signal;
}

void EnvelopeFollower::set_step_time(double step_time) {
	this->step_time = step_time;
}


