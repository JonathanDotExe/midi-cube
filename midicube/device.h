/*
 * device.h
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_DEVICE_H_
#define MIDICUBE_DEVICE_H_

#include <string>
#include "midicube.h"
#include "midi.h"

class MidiInputDevice {

private:
	MidiCube* cube;

public:

	MidiCube* cube();

	void set_cube(MidiCube* cube);

	virtual void add_listener(void (*recieve(MidiMessage& msg))) = 0;

	virtual std::string get_identifier() = 0;

	virtual ~MidiInputDevice();

};

class MidiOutputDevice {

private:
	MidiCube* cube;

public:

	MidiCube* cube();

	void set_cube(MidiCube* cube);

	virtual void send(MidiMessage& message);

	virtual std::string get_identifier() = 0;

	virtual ~MidiOutputDevice();

};

class PortInputDevice : MidiInputDevice{

private:
	MidiInput* input;

public:

	PortInputDevice();

	void add_listener(void (*recieve(MidiMessage& msg)));

	std::string get_identifier();

	~PortInputDevice();

};

class PortOutputDevice {

private:
	MidiOutput* output;

public:

	PortOutputDevice(MidiOutput* output);

	void send(MidiMessage& message);

	std::string get_identifier();

	~PortOutputDevice();

};


#endif /* MIDICUBE_DEVICE_H_ */
