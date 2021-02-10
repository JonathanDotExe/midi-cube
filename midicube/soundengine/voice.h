/*
 * voice.h
 *
 *  Created on: Feb 10, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_VOICE_H_
#define MIDICUBE_SOUNDENGINE_VOICE_H_

#include "../midi.h"
#include "../envelope.h"
#include "../audio.h"
#include "../synthesis.h"
#include "../metronome.h"
#include <string>
#include <array>
#include <functional>

#define SOUND_ENGINE_POLYPHONY 30

class NoteBuffer {
private:
	size_t next_freq_slot(SampleInfo& info) {
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (!note[i].valid) {
				return i;
			}
		}
		//TODO return longest used slot
		return 0;
	}

public:
	std::array<TriggeredNote, SOUND_ENGINE_POLYPHONY> note;

	NoteBuffer() {
		//Init notes
		for (size_t i = 0; i < note.size(); ++i) {
			note[i].start_time = -1024;
			note[i].release_time = -1024;
		}
	}

	void press_note(SampleInfo& info, unsigned int note, double velocity) {
		size_t slot = next_freq_slot(info);
		this->note[slot].freq = note_to_freq(note);
		this->note[slot].velocity = velocity;
		this->note[slot].note = note;
		this->note[slot].pressed = true;
		this->note[slot].start_time = info.time;
		this->note[slot].release_time = 0;
		this->note[slot].phase_shift = 0;
		this->note[slot].valid = true;
	}


	void release_note(SampleInfo& info, unsigned int note, bool invalidate = false) {
		double f = note_to_freq(note);
		for (size_t i = 0; i < SOUND_ENGINE_POLYPHONY; ++i) {
			if (this->note[i].freq == f && this->note[i].pressed) {
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
