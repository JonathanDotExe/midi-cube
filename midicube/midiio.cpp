/*
 * midiio.cpp
 *
 *  Created on: Sep 17, 2021
 *      Author: jojo
 */

#include "midiio.h"

//MidiHandler
unsigned int MidiHandler::available_ports() {
	return rtmidi().getPortCount();
}

std::string MidiHandler::port_name(unsigned int port) {
	std::string name;
	try {
		name = rtmidi().getPortName(port);
	}
	catch (RtMidiError& e) {
		throw MidiException(e.what());
	}
	return name;
}

std::vector<std::string> MidiHandler::available_port_names() {
	unsigned int ports = available_ports();
	std::vector<std::string> names;
	for (unsigned int i = 0; i < ports; ++i) {
		names.push_back(port_name(i));
	}
	return names;
}

void MidiHandler::open(unsigned int port) {
	std::cout << "Opening MIDI port: " << rtmidi().getPortName(port) << std::endl;
	try {
		rtmidi().openPort(port);
	}
	catch (RtMidiError& error) {
		throw MidiException(error.what());
	}
}

MidiHandler::MidiHandler() {

}

MidiHandler::~MidiHandler() {

}


//MidiInput
MidiInput::MidiInput() : MidiHandler::MidiHandler(), midiin(
	#ifdef MIDICUBE_USE_JACK
		RtMidi::Api::UNIX_JACK
	#else
		RtMidi::Api::UNSPECIFIED
	#endif
	, "RtMidi Client", 256
) {
}

RtMidi& MidiInput::rtmidi() {
	return midiin;
}

void MidiInput::open(unsigned int port) {
	MidiHandler::open(port);
	std::cout << "Registering callback" << std::endl;
	try {
		midiin.ignoreTypes(false, false, false);
	}
	catch (RtMidiError& error) {
		throw MidiException(error.what());
	}
}

bool MidiInput::read(MidiMessage* msg) {
	midiin.getMessage(&buffer);
	if (buffer.size()) {
		*msg = {buffer};
		return true;
	}
	return false;
}

void MidiInput::close() {
	midiin.closePort();
}

MidiInput::~MidiInput() {
	close();
}


//MidiOutput
MidiOutput::MidiOutput() : MidiHandler::MidiHandler(), midiout(
	#ifdef MIDICUBE_USE_JACK
		RtMidi::Api::UNIX_JACK
	#endif
) {

}

void MidiOutput::send(MidiMessage& message) {
	try {
		//TODO
		/*std::vector<unsigned char> m = message.get_message();
		midiout->sendMessage(&m);*/
	}
	catch (RtMidiError& error) {
		throw MidiException(error.what());
	}
}

RtMidi& MidiOutput::rtmidi() {
	return midiout;
}

void MidiOutput::close() {
	midiout.closePort();
}

MidiOutput::~MidiOutput() {
	close();
}


