/*
 * envelope.cpp
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#include "envelope.h"

double ADSREnvelope::amplitude(double time, TriggeredNote& note) {
	if (note.pressed) {
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
	else if (time - note.release_time > release) {
		return 0;
	}
	else {
		return sustain - (time - note.release_time)/release * sustain;
	}

	return 0;
}

