/*
 * midi.h
 *
 *  Created on: 21 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDI_H_
#define MIDICUBE_MIDI_H_

#include <string>
#include <vector>
#include <exception>
#include <functional>

#define SOUND_ENGINE_MIDI_CHANNELS 16
#define MIDI_CONTROL_COUNT 128

#define NOTE_OFF_BIT 0x8
#define NOTE_ON_BIT 0x9
#define POLYPHONIC_AFTERTOUCH_BIT 0xA
#define CONTROL_CHANGE_BIT 0xB
#define PROGRAM_CHANGE_BIT 0xC
#define MONOPHONIC_AFTERTOUCH_BIT 0xD
#define PITCH_BEND_BIT 0xE
#define SYSEX_BIT 0xF

struct SampleInfo {
	double time;
	double time_step;
	unsigned int sample_rate;
	unsigned int sample_time;
	double input_sample;
};

class MidiException: public std::exception {

private:
	const char *cause;

public:
	MidiException(const char *cause) {
		this->cause = cause;
	}

	const char* what() const throw () {
		return cause;
	}
};

enum MessageType {
	NOTE_OFF = NOTE_OFF_BIT,
	NOTE_ON = NOTE_ON_BIT,
	POLYPHONIC_AFTERTOUCH = POLYPHONIC_AFTERTOUCH_BIT,
	CONTROL_CHANGE = CONTROL_CHANGE_BIT,
	PROGRAM_CHANGE = PROGRAM_CHANGE_BIT,
	MONOPHONIC_AFTERTOUCH = MONOPHONIC_AFTERTOUCH_BIT,
	PITCH_BEND = PITCH_BEND_BIT,
	SYSEX = SYSEX_BIT,
	INVALID
};

//Not SYSEX compatible
class MidiMessage {

public:
	unsigned int channel = 0;
	MessageType type = INVALID;
	unsigned int first_data = 0;
	unsigned int second_data = 0;

	MidiMessage(std::vector<unsigned char> message);

	MidiMessage() {

	}

	/**
	 * NOTE_ON, NOTE_OFF, POLYPHONIC_AFTERTOUCH
	 */
	inline unsigned int& note() {
		return first_data;
	}

	/**
	 * NOTE_ON, NOTE_OFF
	 */
	inline unsigned int& velocity() {
		return second_data;
	}

	/**
	 * POLYPHONIC_AFTERTOUCH
	 */
	inline unsigned int& polyphonic_aftertouch() {
		return second_data;
	}

	/**
	 * MONOPHONIC_AFTERTOUCH
	 */
	inline unsigned int& monophonic_aftertouch() {
		return first_data;
	}

	/**
	 * PROGRAM_CHANGE
	 */
	inline unsigned int& program(){
		return first_data;
	}

	/**
	 * CONTROL_CHANGE
	 */
	inline unsigned int& control() {
		return first_data;
	}

	/**
	 * CONTROL_CHANGE
	 */
	inline unsigned int& value() {
		return second_data;
	}

	/**
	 * PITCH_BEND
	 */
	inline unsigned int get_pitch_bend() {
		return (second_data << 7) + first_data;
	}

	/**
	 * PITCH_BEND
	 */
	inline void set_pitch_bend(unsigned int v) {
		first_data  = v & 0x07;
		second_data = v & 0xF8;
	}

	std::string to_string();

};


#endif /* MIDICUBE_MIDI_H_ */
