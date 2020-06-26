/*
 * midi.cpp
 *
 *  Created on: 21 Jun 2020
 *      Author: jojo
 */


#include "midi.h"
#include <exception>

#define NOTE_OFF_BIT 0x8
#define NOTE_ON_BIT 0x9
#define POLYPHONIC_AFTERTOUCH_BIT 0xA
#define CONTROL_CHANGE_BIT 0xB
#define PROGRAM_CHANGE_BIT 0xC
#define MONOPHONIC_AFTERTOUCH_BIT 0xD
#define PITCH_BEND_BIT 0xE
#define SYSEX_BIT 0xF


//MidiMessage
MidiMessage::MidiMessage(std::vector<unsigned char> message) {
	this->message.swap(message);
}

unsigned char MidiMessage::get_status_channel_byte () {
	return message.at(0);
}

unsigned char MidiMessage::get_message_type_bits () {
	return get_status_channel_byte() >> 4 & 0x0F;
}

unsigned char MidiMessage::get_channel_bits () {
	return get_status_channel_byte() & 0x0F;
}

unsigned char MidiMessage::get_first_data_byte () {
	return message.at(1) & 0x7F;
}

unsigned char MidiMessage::get_second_data_byte () {
	return message.at(2) & 0x7F;
}

MessageType MidiMessage::get_message_type() {
	switch (get_message_type_bits()) {
	case NOTE_OFF_BIT:
		return MessageType::NOTE_OFF;
	case NOTE_ON_BIT:
		return MessageType::NOTE_ON;
	case POLYPHONIC_AFTERTOUCH_BIT:
		return MessageType::POLYPHONIC_AFTERTOUCH;
	case CONTROL_CHANGE_BIT:
		return MessageType::CONTROL_CHANGE;
	case PROGRAM_CHANGE_BIT:
		return MessageType::PROGRAM_CHANGE;
	case MONOPHONIC_AFTERTOUCH_BIT:
		return MessageType::MONOPHONIC_AFTERTOUCH;
	case PITCH_BEND_BIT:
		return MessageType::PITCH_BEND;
	case SYSEX_BIT:
		return MessageType::SYSEX;
	}

	return MessageType::INVALID;
}

unsigned int MidiMessage::get_channel () {
	return get_channel_bits();
}

unsigned int MidiMessage::get_note () {
	return get_first_data_byte();
}

unsigned int MidiMessage::get_velocity () {
	return get_second_data_byte();
}

unsigned int MidiMessage::get_polyphonic_aftertouch () {
	return get_second_data_byte();
}

unsigned int MidiMessage::get_monophonic_aftertouch () {
	return get_first_data_byte();
}

unsigned int MidiMessage::get_program () {
	return get_first_data_byte();
}

unsigned int MidiMessage::get_control () {
	return get_first_data_byte();
}

unsigned int MidiMessage::get_value () {
	return get_second_data_byte();
}

unsigned int MidiMessage::get_pitch_bend () {
	return (get_second_data_byte() << 7) + get_first_data_byte();
}

size_t MidiMessage::get_message_length () {
	return message.size();
}

std::vector<unsigned char> MidiMessage::get_message() {
	return message;
}

std::string MidiMessage::to_string() {
	using namespace std::string_literals;

	switch (get_message_type()) {
	case MessageType::NOTE_OFF:
		return "[NOTE_OFF Message note="s + std::to_string(get_note()) + "]"s;
	case MessageType::NOTE_ON:
			return "[NOTE_ON Message note="s + std::to_string(get_note()) + " velocity="s + std::to_string(get_velocity()) + "]"s;
	case MessageType::POLYPHONIC_AFTERTOUCH:
			return "[POLYPHONIC_AFTERTOUCH Message note="s + std::to_string(get_note()) + " touch="s + std::to_string(get_polyphonic_aftertouch()) + "]"s;
	case MessageType::CONTROL_CHANGE:
			return "[CONTROL_CHANGE Message control="s + std::to_string(get_control()) + " value="s + std::to_string(get_value()) + "]"s;
	case MessageType::PROGRAM_CHANGE:
			return "[PROGRAM_CHANGE Message program="s + std::to_string(get_program()) + "]"s;
	case MessageType::MONOPHONIC_AFTERTOUCH:
			return "[MONOPHONIC_AFTERTOUCH Message touch="s + std::to_string(get_monophonic_aftertouch()) + "]"s;
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
	std::cout << "Opening port!" << std::endl;
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
	((MidiInput*) arg)->call_callback(delta, msg);
}

//MidiInput
MidiInput::MidiInput() : MidiHandler::MidiHandler() {
	try {
		midiin = new RtMidiIn();
	}
	catch (RtMidiError& error) {
		throw MidiException(error.what());
	}
}

void MidiInput::call_callback(double delta, std::vector<unsigned char>* msg) {
	MidiMessage message(*msg);
	callback(delta, message, user_data);
}

void MidiInput::set_callback(void (*callback)(double deltatime, MidiMessage&, void*), void* user_data) {
	this->callback = callback;
	this->user_data = user_data;
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
	midiout = new RtMidiOut();
}

void MidiOutput::send(MidiMessage& message) {
	try {
		std::vector<unsigned char> m = message.get_message();
		midiout->sendMessage(&m);
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



