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
#include <functional>

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

	unsigned char get_status_channel_byte();

	unsigned char get_message_type_bits();

	unsigned char get_channel_bits();

	unsigned char get_first_data_byte();

	unsigned char get_second_data_byte();

	void set_status_channel_byte(unsigned char v);

	void set_message_type_bits(unsigned char v);

	void set_channel_bits(unsigned char v);

	void set_first_data_byte(unsigned char v);

	void set_second_data_byte(unsigned char v);

public:
	MidiMessage(std::vector<unsigned char> message);

	MessageType get_message_type();

	/**
	 * Every type except SYSEX and INVALID
	 */
	unsigned int get_channel();

	/**
	 * NOTE_ON, NOTE_OFF, POLYPHONIC_AFTERTOUCH
	 */
	unsigned int get_note();

	/**
	 * NOTE_ON, NOTE_OFF
	 */
	unsigned int get_velocity();

	/**
	 * POLYPHONIC_AFTERTOUCH
	 */
	unsigned int get_polyphonic_aftertouch();

	/**
	 * MONOPHONIC_AFTERTOUCH
	 */
	unsigned int get_monophonic_aftertouch();

	/**
	 * PROGRAM_CHANGE
	 */
	unsigned int get_program();

	/**
	 * CONTROL_CHANGE
	 */
	unsigned int get_control();

	/**
	 * CONTROL_CHANGE
	 */
	unsigned int get_value();

	/**
	 * PITCH_BEND
	 */
	unsigned int get_pitch_bend();


	void set_message_type(MessageType type);

	/**
	 * Every type except SYSEX and INVALID
	 */
	void set_channel(unsigned int v);

	/**
	 * NOTE_ON, NOTE_OFF, POLYPHONIC_AFTERTOUCH
	 */
	void set_note(unsigned int v);

	/**
	 * NOTE_ON, NOTE_OFF
	 */
	void set_velocity(unsigned int v);

	/**
	 * POLYPHONIC_AFTERTOUCH
	 */
	void set_polyphonic_aftertouch(unsigned int v);

	/**
	 * MONOPHONIC_AFTERTOUCH
	 */
	void set_monophonic_aftertouch(unsigned int v);

	/**
	 * PROGRAM_CHANGE
	 */
	void set_program(unsigned int v);

	/**
	 * CONTROL_CHANGE
	 */
	void set_control(unsigned int v);

	/**
	 * CONTROL_CHANGE
	 */
	void set_value(unsigned int v);

	/**
	 * PITCH_BEND
	 */
	void set_pitch_bend(unsigned int v);

	/**
	 * SYSEX
	 */
	size_t get_message_length();

	std::vector<unsigned char> get_message();

	std::string to_string();

};

class MidiHandler {

public:

	MidiHandler();

	virtual ~MidiHandler();

	unsigned int available_ports();

	/**
	 * Can throw MidiException
	 */
	std::string port_name(unsigned int port);

	/**
	 * Can throw MidiException
	 */
	std::vector<std::string> available_port_names();

	/**
	 * Can throw MidiException
	 */
	virtual void open(unsigned int port);

	virtual void close() = 0;

protected:
	virtual RtMidi& rtmidi() = 0;

};

class MidiInput : public MidiHandler {

private:
	std::function<void(double, MidiMessage&)> callback = nullptr;
	RtMidiIn *midiin = nullptr;

public:

	MidiInput();

	~MidiInput();

	void call_callback(double deltatime, std::vector<unsigned char>* msg);

	void set_callback(std::function<void(double, MidiMessage&)>);

	void open(unsigned int port);

	void close();

protected:
	RtMidi& rtmidi();

};

class MidiOutput : public MidiHandler {

private:
	RtMidiOut *midiout = nullptr;

public:

	MidiOutput();

	~MidiOutput();

	/**
	 * Can throw MidiException
	 */
	void send(MidiMessage &message);

	void close();

protected:
	RtMidi& rtmidi();

};

#endif /* MIDICUBE_MIDI_H_ */
