/*
 * device.h
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_DEVICE_H_
#define MIDICUBE_DEVICE_H_

#include <string>
#include <vector>
#include "midi.h"
#include "audio.h"

enum class DeviceType {
	MIDI_INPUT, MIDI_OUTPUT, SOUND_ENGINE
};


class AudioDevice {

private:
	//MidiCube* cube = nullptr;

public:

	AudioDevice();

	//MidiCube* get_cube();

	//void set_cube(MidiCube* cube);

	/**
	 * For MIDI inputs
	 */
	virtual void set_midi_callback(void (*recieve) (double, MidiMessage&, void*), void*) {

	}

	/**
	 * For MIDI outputs
	 */
	virtual void send(MidiMessage& message) {

	}

	/**
	 * For Audio Inputs
	 */
	virtual void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {

	}

	virtual bool is_midi_input() {
		return false;
	}

	virtual bool is_midi_output() {
		return false;
	}

	virtual bool is_audio_input() {
		return false;
	}

	virtual DeviceType type() = 0;

	virtual std::string get_identifier() = 0;

	virtual ~AudioDevice();

};

class PortInputDevice : public AudioDevice {

private:
	MidiInput* input;
	std::string identifier;

public:

	PortInputDevice(MidiInput* input, std::string identifier);

	void set_midi_callback(void (*recieve) (double, MidiMessage&, void*), void*);

	void call_listeners (MidiMessage& msg);

	std::string get_identifier();

	DeviceType type() {
		return DeviceType::MIDI_INPUT;
	};

	bool is_midi_input() {
		return true;
	}

	~PortInputDevice();

};

class PortOutputDevice : public AudioDevice {

private:
	MidiOutput* output;
	std::string identifier;
public:

	PortOutputDevice(MidiOutput* output, std::string identifier);

	void send(MidiMessage& message);

	std::string get_identifier();

	DeviceType type() {
		return DeviceType::MIDI_OUTPUT;
	};

	bool is_midi_output() {
		return true;
	}

	~PortOutputDevice();

};

#endif /* MIDICUBE_DEVICE_H_ */
