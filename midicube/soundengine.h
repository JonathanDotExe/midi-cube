/*
 * soundengine.h
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_H_

#include "device.h"
#include "midi.h"
#include <string>
#include <array>

#define SOUND_ENGINE_POLYPHONY 30


class SoundEngine {

public:
	virtual double process_sample(unsigned int channel, double time, double freq) = 0;

	virtual std::string get_name() = 0;

	virtual ~SoundEngine() {

	}

};

class PresetSynth : public SoundEngine {

private:
	double detune;
	double ndetune;

public:

	PresetSynth();

	double process_sample(unsigned int channel, double time, double freq);

	std::string get_name();

};

#define ORGAN_DRAWBAR_COUNT 9

struct B3OrganData {
	std::array<int, ORGAN_DRAWBAR_COUNT> drawbars = {8, 8, 8, 8, 8, 8, 8, 8, 8};
};

class B3Organ : public SoundEngine {

private:
	B3OrganData data;
	std::array<double, ORGAN_DRAWBAR_COUNT> drawbar_harmonics;

public:
	B3Organ();

	double process_sample(unsigned int channel, double time, double freq);

	std::string get_name();

};

class SoundEngineDevice : public AudioDevice {

private:
	std::string identifier;
	double freq[SOUND_ENGINE_POLYPHONY] = {};
	double amplitude[SOUND_ENGINE_POLYPHONY] = {};
	SoundEngine* engine;

	size_t next_freq_slot();

public:

	SoundEngineDevice(std::string identifier);

	std::string get_identifier();

	bool is_audio_input() {
		return true;
	}

	void send(MidiMessage& message);

	double process_sample(unsigned int channel, double time);

	~SoundEngineDevice();

};




#endif /* MIDICUBE_SOUNDENGINE_H_ */
