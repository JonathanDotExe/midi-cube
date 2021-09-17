/*
 * midiio.h
 *
 *  Created on: Sep 17, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDIIO_H_
#define MIDICUBE_MIDIIO_H_

#include "midi.h"
#include <rtmidi/RtMidi.h>
#include <vector>
#include <exception>
#include <functional>

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
	RtMidiIn midiin;
	std::vector<unsigned char> buffer;

public:

	MidiInput();

	~MidiInput();

	void open(unsigned int port);

	bool read(MidiMessage* msg);

	void close();

protected:
	RtMidi& rtmidi();

};

class MidiOutput : public MidiHandler {

private:
	RtMidiOut midiout;

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

#endif
