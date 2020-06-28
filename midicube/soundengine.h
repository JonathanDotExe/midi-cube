/*
 * soundengine.h
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_H_
#define MIDICUBE_SOUNDENGINE_H_

#define SOUND_ENGINE_POLYPHONY 30

#include "device.h"
#include "midi.h"
#include <string>

class SoundEngineDevice : public AudioDevice {

private:
	std::string identifier;
	double freq[SOUND_ENGINE_POLYPHONY];
	double amplitude[SOUND_ENGINE_POLYPHONY];

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
