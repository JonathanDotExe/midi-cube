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

enum MessageType {
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
