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

	size_t press_note(const SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit = 0) {
		size_t slot = next_freq_slot(info, polyphony_limit);
		this->note[slot].freq = note_to_freq(note);
		this->note[slot].velocity = velocity;
		this->note[slot].real_note = real_note;
		this->note[slot].note = note;
		this->note[slot].pressed = true;
		this->note[slot].start_time = info.time;
		this->note[slot].release_time = 0;
		this->note[slot].phase_shift = 0;
		this->note[slot].valid = true;
		return slot;
	}


	void release_note(const SampleInfo& info, unsigned int real_note, bool invalidate = false) {
		for (size_t i = 0; i < P; ++i) {
			if (this->note[i].real_note == real_note && this->note[i].pressed) {
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
	bool repeat_edges = false;
	bool kb_sync = true;
	bool play_duplicates = false;
	bool master_sync = false;
	bool sustain = false;
};

class Arpeggiator {

private:
	unsigned int curr_note = 0;
	std::size_t data_index = 0;
	std::size_t note_index = 0;
	bool restart = true;
	bool second = false;
	VoiceManager<TriggeredNote, ARPEGGIATOR_POLYPHONY> note;

public:
	bool on = false;
	ArpeggiatorPreset preset;
	Metronome metronome;

	Arpeggiator();

	void apply(SampleInfo& info, Metronome& master, std::function<void(SampleInfo&, unsigned int, double)> press, std::function<void(SampleInfo&, unsigned int)> release, bool sustain);

	void press_note(SampleInfo& info, unsigned int note, double velocity, bool sustain);

	void release_note(SampleInfo& info, unsigned int note, bool sustain);

};

#endif /* MIDICUBE_SOUNDENGINE_VOICE_H_ */
