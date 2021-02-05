/*
 * midi.cpp
 *
 *  Created on: 21 Jun 2020
 *      Author: jojo
 */


#include "midi.h"
#include <exception>

//MidiMessage
MidiMessage::MidiMessage(std::vector<unsigned char> message) {
	//Parse message
	size_t size = message.size();
	if (size > 0) {
		channel = (message[0] & 0x0F);
		type = static_cast<MessageType>(message[0] >> 4 & 0x0F);
		if (size > 1) {
			first_data = message[1] & 0x7F;
			if (size > 2) {
				second_data =  message[2] & 0x7F;
			}
		}
	}
}

std::string MidiMessage::to_string() {
	using namespace std::string_literals;

	switch (type) {
	case MessageType::NOTE_OFF:
		return "[NOTE_OFF Message note="s + std::to_string(note()) + "]"s;
	case MessageType::NOTE_ON:
			return "[NOTE_ON Message note="s + std::to_string(note()) + " velocity="s + std::to_string(velocity()) + "]"s;
	case MessageType::POLYPHONIC_AFTERTOUCH:
			return "[POLYPHONIC_AFTERTOUCH Message note="s + std::to_string(note()) + " touch="s + std::to_string(polyphonic_aftertouch()) + "]"s;
	case MessageType::CONTROL_CHANGE:
			return "[CONTROL_CHANGE Message control="s + std::to_string(control()) + " value="s + std::to_string(value()) + "]"s;
	case MessageType::PROGRAM_CHANGE:
			return "[PROGRAM_CHANGE Message program="s + std::to_string(program()) + "]"s;
	case MessageType::MONOPHONIC_AFTERTOUCH:
			return "[MONOPHONIC_AFTERTOUCH Message touch="s + std::to_string(monophonic_aftertouch()) + "]"s;
	case MessageType::PITCH_BEND:
			return "[PITCH_BEND Message pitch_bend="s + std::to_string(get_pitch_bend()) + "]"s;
	case MessageType::SYSEX:
			return "[SYSEX Message]";
	default:
		break;

	}
	return "[Invalid Message]";
}


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


void input_callback (double delta, std::vector<unsigned char>* msg, void* arg) {
	static_cast<MidiInput*>(arg)->call_callback(delta, msg);
}

//MidiInput
MidiInput::MidiInput() : MidiHandler::MidiHandler() {
	try {
		midiin = new RtMidiIn(
			#ifdef MIDICUBE_USE_JACK
				RtMidi::Api::UNIX_JACK
			#endif
		);
	}
	catch (RtMidiError& error) {
		throw MidiException(error.what());
	}
}

void MidiInput::call_callback(double delta, std::vector<unsigned char>* msg) {
	MidiMessage message(*msg);
	callback(delta, message);
}

void MidiInput::set_callback(std::function<void(double, MidiMessage&)> callback) {
	this->callback = callback;
}

RtMidi& MidiInput::rtmidi() {
	return *midiin;
}

void MidiInput::open(unsigned int port) {
	MidiHandler::open(port);
	std::cout << "Registering callback" << std::endl;
	try {
		midiin->setCallback(&input_callback, this);
		midiin->ignoreTypes(false, false, false);
	}
	catch (RtMidiError& error) {
		throw MidiException(error.what());
	}
}

void MidiInput::close() {
	delete midiin;
	midiin = nullptr;
}

MidiInput::~MidiInput() {
	close();
}


//MidiOutput
MidiOutput::MidiOutput() : MidiHandler::MidiHandler() {
	midiout = new RtMidiOut(
		#ifdef MIDICUBE_USE_JACK
			RtMidi::Api::UNIX_JACK
		#endif
	);
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
	return *midiout;
}

void MidiOutput::close() {
	delete midiout;
	midiout = nullptr;
}

MidiOutput::~MidiOutput() {
	close();
}



