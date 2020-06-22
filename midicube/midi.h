/*
 * midi.h
 *
 *  Created on: 21 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDI_H_
#define MIDICUBE_MIDI_H_

#include <rtmidi/RtMidi.h>
#include <vector>
#include <exception>

class MidiException : public std::exception {

private:
	const char* cause;

public:
	MidiException(const char* cause) {
		this->cause = cause;
	}

	const char* what() const throw() {
		return cause;
	}
};

enum class MessageType {
	NOTE_OFF,
	NOTE_ON,
	POLYPHONIC_AFTERTOUCH,
	CONTROL_CHANGE,
	PROGRAM_CHANGE,
	MONOPHONIC_AFTERTOUCH,
	PITCH_BEND,
	SYSEX,
	INVALID
};

class MidiMessage {

private:
	std::vector<unsigned char> message;

	unsigned char get_status_channel_byte ();

	unsigned char get_message_type_bits ();

	unsigned char get_channel_bits ();

	unsigned char get_first_data_byte ();

	unsigned char get_second_data_byte ();

public:
	MidiMessage(std::vector<unsigned char> message);

	MessageType get_message_type();

	/**
	 * Every type except SYSEX and INVALID
	 */
	unsigned int get_channel ();

	/**
	 * NOTE_ON, NOTE_OFF, POLYPHONIC_AFTERTOUCH
	 */
	unsigned int get_note ();

	/**
	 * NOTE_ON, NOTE_OFF
	 */
	unsigned int get_velocity ();

	/**
	 * POLYPHONIC_AFTERTOUCH
	 */
	unsigned int get_polyphonic_aftertouch ();

	/**
	 * MONOPHONIC_AFTERTOUCH
	 */
	unsigned int get_monophonic_aftertouch ();


	/**
	 * PROGRAM_CHANGE
	 */
	unsigned int get_programm ();

	/**
	 * CONTROL_CHANGE
	 */
	unsigned int get_control ();

	/**
	 * CONTROL_CHANGE
	 */
	unsigned int get_value ();

	/**
	 * PITCH_BEND
	 */
	unsigned int get_pitch_bend ();

	/**
	 * SYSEX
	 */
	size_t get_message_length ();

};


class MidiInput {

private:
	void (*callback) (double deltatime, MidiMessage&) = nullptr;
	RtMidiIn *midiin = nullptr;

public:

	unsigned int available_ports();

	void port_name(unsigned int port);

	std::vector<std::string> available_port_names();

	void open(unsigned int port);

	void close();

};

class MidiOutput {

private:
	void (*callback) (double deltatime, MidiMessage&) = nullptr;
	RtMidiOut *midiout = nullptr;

public:

	unsigned int available_ports();

	/**
	 * Can throw MidiException
	 */
	void port_name(unsigned int port);

	/**
	 * Can throw MidiException
	 */
	std::vector<std::string> available_port_names();

	/**
	 * Can throw MidiException
	 */
	void open(unsigned int port);

	/**
	 * Can throw MidiException
	 */
	void send(MidiMessage& message);

	void close();

};



#endif /* MIDICUBE_MIDI_H_ */
