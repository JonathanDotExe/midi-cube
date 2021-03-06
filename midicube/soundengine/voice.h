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

template<typename V, size_t P>
class VoiceManager {
private:
	size_t next_freq_slot(SampleInfo& info) {
		for (size_t i = 0; i < P; ++i) {
			if (!note[i].valid) {
				return i;
			}
		}
		//TODO return longest used slot
		return 0;
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

	size_t press_note(SampleInfo& info, unsigned int note, double velocity) {
		size_t slot = next_freq_slot(info);
		this->note[slot].freq = note_to_freq(note);
		this->note[slot].velocity = velocity;
		this->note[slot].note = note;
		this->note[slot].pressed = true;
		this->note[slot].start_time = info.time;
		this->note[slot].release_time = 0;
		this->note[slot].phase_shift = 0;
		this->note[slot].valid = true;
		return slot;
	}


	void release_note(SampleInfo& info, unsigned int note, bool invalidate = false) {
		double f = note_to_freq(note);
		for (size_t i = 0; i < P; ++i) {
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

#define ARPEGGIATOR_POLYPHONY 30

enum ArpeggiatorPattern {
	ARP_UP, ARP_DOWN, ARP_RANDOM, ARP_UP_DOWN, ARP_DOWN_UP
};

struct ArpeggiatorPreset {
	ArpeggiatorPattern pattern;
	unsigned int octaves = 1;
	unsigned int value = 1;
	bool hold = false;
};

class Arpeggiator {

private:
	unsigned int curr_note = 0;
	std::size_t data_index = 0;
	std::size_t note_index = 0;
	bool restart = true;

public:
	bool on = false;
	ArpeggiatorPreset preset;
	VoiceManager<TriggeredNote, ARPEGGIATOR_POLYPHONY> note;
	Metronome metronome;

	Arpeggiator();

	void apply(SampleInfo& info, std::function<void(SampleInfo&, unsigned int, double)> press, std::function<void(SampleInfo&, unsigned int)> release);

	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

};

#endif /* MIDICUBE_SOUNDENGINE_VOICE_H_ */
