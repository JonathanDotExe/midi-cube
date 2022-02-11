/*
 * voice.h
 *
 *  Created on: Feb 10, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_VOICE_H_
#define MIDICUBE_SOUNDENGINE_VOICE_H_

#include "../core/midi.h"
#include "../dsp/envelope.h"
#include "../dsp/synthesis.h"
#include "../core/metronome.h"
#include <string>
#include <array>
#include <functional>
#include <limits>

enum VoiceState {
	VOICE_PRESSED, VOICE_RELEASED, VOICE_STOLEN, VOICE_INACTIVE
};

struct Voice {
	VoiceState state = VOICE_INACTIVE;
	double start_time = 0;
	double release_time = 0;
	unsigned int real_note = 0;
	unsigned int note = 0;
	double freq = 0;
	double velocity = 0;
	double aftertouch = 0;
	unsigned int channel = 0;
};


template<typename V, size_t P>
class VoiceManager {
private:
	inline size_t next_freq_slot() {
		bool release = false;
		size_t longest_index = 0;
		double longest_time = std::numeric_limits<double>::max();
		for (size_t i = 0; i < P; ++i) {
			if (note[i].state == VOICE_INACTIVE) {
				return i;
			}
			else {
				if (!note[i].state == VOICE_PRESSED) {
					if (release) {
						if (longest_time > note[i].release_time) {
							longest_time = note[i].release_time;
							longest_index = i;
						}
					}
					else {
						release = true;
						longest_time = note[i].release_time;
						longest_index = i;
					}
				}
				else if (!release && longest_time > note[i].start_time) {
					longest_time = note[i].start_time;
					longest_index = i;
				}
			}
		}
		return longest_index;
	}

	inline size_t next_longest_slot() {
		bool release = false;
		size_t longest_index = 0;
		double longest_time = std::numeric_limits<double>::max();
		for (size_t i = 0; i < P; ++i) {
			if (note[i].state == VOICE_RELEASED) {
				if (release) {
					if (longest_time > note[i].release_time) {
						longest_time = note[i].release_time;
						longest_index = i;
					}
				}
				else {
					release = true;
					longest_time = note[i].release_time;
					longest_index = i;
				}
			}
			else if (note[i].state == VOICE_PRESSED && !release && longest_time > note[i].start_time) {
				longest_time = note[i].start_time;
				longest_index = i;
			}
		}
		return longest_index;
	}

	inline void check_polyphony(double time, size_t polyphony_limit = 0) {
		size_t size = P;
		if (polyphony_limit > 0) {
			size = std::min(polyphony_limit, P);
		}

		if (size < P) {
			//TODO smooth release
			//Count
			size_t count = 0;
			for (size_t i = 0; i < P; ++i) {
				if (note[i].state == VOICE_PRESSED || note[i].state == VOICE_RELEASED) {
					++count;
				}
			}
			//Invalidate
			while (count > size) {
				size_t slot = next_longest_slot();
				this->note[slot].state = VOICE_STOLEN;
				this->note[slot].release_time = time;
			}
		}
	}

public:
	std::array<V, P> note;

	VoiceManager() {
		//Init notes
		for (size_t i = 0; i < note.size(); ++i) {
			note[i].start_time = -1024;
			note[i].release_time = -1024;
		}
	}

	size_t press_note(const SampleInfo& info, unsigned int real_note, unsigned int note, unsigned int channel, double velocity, size_t polyphony_limit = 0) {
		//Next slot and note
		size_t slot = next_freq_slot();
		this->note[slot].freq = note_to_freq(note);
		this->note[slot].velocity = velocity;
		this->note[slot].real_note = real_note;
		this->note[slot].note = note;
		this->note[slot].state = VOICE_PRESSED;
		this->note[slot].start_time = info.time;
		this->note[slot].release_time = 0;
		this->note[slot].channel = channel;
		//Check polyphony
		check_polyphony(info.time, polyphony_limit);
		return slot;
	}

	void release_note(const SampleInfo& info, unsigned int real_note, unsigned int channel, bool invalidate = false) {
		for (size_t i = 0; i < P; ++i) {
			if (this->note[i].real_note == real_note && this->note[i].channel == channel && this->note[i].state == VOICE_PRESSED) {
				this->note[i].state = invalidate ? VOICE_INACTIVE : VOICE_RELEASED;
				this->note[i].release_time = info.time;
			}
		}
	}

};



#endif /* MIDICUBE_SOUNDENGINE_VOICE_H_ */
