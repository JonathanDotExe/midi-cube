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

template<typename V, size_t P>
class VoiceManager {
private:
	size_t next_freq_slot(const SampleInfo& info, size_t polyphony_limit = 0) {
		size_t size = P;
		if (polyphony_limit > 0) {
			size = std::min(polyphony_limit, P);
		}
		bool release = false;
		size_t longest_index = 0;
		double longest_time = info.time + 1;
		for (size_t i = 0; i < size; ++i) {
			if (!note[i].valid) {
				return i;
			}
			else {
				if (!note[i].pressed) {
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
		size_t slot = next_freq_slot(info, polyphony_limit);
		this->note[slot].freq = note_to_freq(note);
		this->note[slot].velocity = velocity;
		this->note[slot].real_note = real_note;
		this->note[slot].note = note;
		this->note[slot].pressed = true;
		this->note[slot].start_time = info.time;
		this->note[slot].release_time = 0;
		this->note[slot].valid = true;
		this->note[slot].channel = channel;
		return slot;
	}


	void release_note(const SampleInfo& info, unsigned int real_note, unsigned int channel, bool invalidate = false) {
		for (size_t i = 0; i < P; ++i) {
			if (this->note[i].real_note == real_note && this->note[i].channel == channel && this->note[i].pressed) {
				this->note[i].pressed = false;
				this->note[i].release_time = info.time;
				if (invalidate) {
					this->note[i].valid = false;
				}
			}
		}
	}

};



#endif /* MIDICUBE_SOUNDENGINE_VOICE_H_ */
