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
	//Set type to note if if velocity is 0
	if (type == MessageType::NOTE_ON && velocity() == 0) {
		type = MessageType::NOTE_OFF;
	}
}

std::string MidiMessage::to_string() const {
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
			return "[PITCH_BEND Message pitch_bend="s + std::to_string(pitch_bend()) + "]"s;
	case MessageType::SYSEX:
			return "[SYSEX Message]";
	default:
		break;

	}
	return "[Invalid Message]";
}
