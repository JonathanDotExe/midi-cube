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
	virtual double process_sample(unsigned int channel, double time) {
		return 0.0;
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

	bool is_midi_output() {
		return true;
	}

	~PortOutputDevice();

};

class SoundEngineDevice : public AudioDevice {

private:
	std::string identifier;
public:

	SoundEngineDevice(std::string identifier);

	std::string get_identifier();

	bool is_audio_input() {
		return true;
	}

	double process_sample(unsigned int channel, double time);

	~SoundEngineDevice();

};


#endif /* MIDICUBE_DEVICE_H_ */
