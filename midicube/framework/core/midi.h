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
	inline unsigned int note() const{
		return first_data;
	}

	inline void set_note(unsigned int value) {
		first_data = value;
	}

	/**
	 * NOTE_ON, NOTE_OFF
	 */
	inline unsigned int velocity() const{
		return second_data;
	}

	inline void set_velocity(unsigned int value) {
		second_data = value;
	}

	/**
	 * POLYPHONIC_AFTERTOUCH
	 */
	inline unsigned int polyphonic_aftertouch() const{
		return second_data;
	}
	inline void set_polyphonic_aftertouch(unsigned int value) {
		second_data = value;
	}

	/**
	 * MONOPHONIC_AFTERTOUCH
	 */
	inline unsigned int monophonic_aftertouch() const{
		return first_data;
	}
	inline void set_monoponic_aftertouch(unsigned int value) {
		first_data = value;
	}

	/**
	 * PROGRAM_CHANGE
	 */
	inline unsigned int program() const{
		return first_data;
	}
	inline void set_program(unsigned int value) {
		first_data = value;
	}

	/**
	 * CONTROL_CHANGE
	 */
	inline unsigned int control() const{
		return first_data;
	}
	inline void set_control(unsigned int value) {
		first_data = value;
	}

	/**
	 * CONTROL_CHANGE
	 */
	inline unsigned int value() const{
		return second_data;
	}
	inline void set_value(unsigned int value) {
		second_data = value;
	}

	/**
	 * PITCH_BEND
	 */
	inline unsigned int pitch_bend() const{
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

struct ControlBank {
	std::vector<unsigned int> sliders;
	std::vector<unsigned int> knobs;
	std::vector<unsigned int> buttons;
};

enum ControlType {
	SLIDER, KNOB, BUTTON, SCENE_BUTTON, MOD_WHEEL, BREATH_CONTROLLER, VOLUME_PEDAL, EXPRESSION_PEDAL, SUSTAIN_PEDAL, SOSTENUTO_PEDAL, SOFT_PEDAL
};

struct MidiControls {
	std::vector<ControlBank> control_banks;
	std::vector<unsigned int> scene_buttons;
	unsigned int mod_wheel = 1;
	unsigned int breath_controller = 2;
	unsigned int volume_pedal = 7;
	unsigned int expresion_pedal = 11;
	unsigned int sustain_pedal = 64;
	unsigned int sostenuto_pedal = 66;
	unsigned int soft_pedal = 67;

	unsigned int get_cc(ControlType type, size_t index, size_t bank) {
		switch (type) {
		case SLIDER:
			if (bank < control_banks.size()) {
				ControlBank& b = control_banks[bank];
				if (index < b.sliders.size()) {
					return b.sliders[index];
				}
			}
			return 128;
		case KNOB:
			if (bank < control_banks.size()) {
				ControlBank& b = control_banks[bank];
				if (index < b.knobs.size()) {
					return b.knobs[index];
				}
			}
			return 128;
		case BUTTON:
			if (bank < control_banks.size()) {
				ControlBank& b = control_banks[bank];
				if (index < b.buttons.size()) {
					return b.buttons[index];
				}
			}
			return 128;
		case SCENE_BUTTON:
			if (index < scene_buttons.size()) {
				return scene_buttons[index];
			}
			return 128;
		case MOD_WHEEL:
			return mod_wheel;
		case BREATH_CONTROLLER:
			return breath_controller;
		case VOLUME_PEDAL:
			return volume_pedal;
		case EXPRESSION_PEDAL:
			return expresion_pedal;
		case SUSTAIN_PEDAL:
			return sustain_pedal;
		case SOSTENUTO_PEDAL:
			return sostenuto_pedal;
		case SOFT_PEDAL:
			return soft_pedal;
		}
		return 128;
	}
};


#endif /* MIDICUBE_MIDI_H_ */
