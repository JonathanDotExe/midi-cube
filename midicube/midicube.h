/*
 * midicube.h
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MIDICUBE_H_
#define MIDICUBE_MIDICUBE_H_

#include "audio.h"
#include "device.h"
#include "soundengine/soundengine.h"

struct MidiCubeInput {
	MidiInput* in = nullptr;
	std::string name = "";
};

struct ChannelSource {
	ssize_t input = -1;
	unsigned int channel;
	unsigned int start_note = 0;
	unsigned int end_note = 127;
	int octave = 0;
	bool transfer_channel_aftertouch = true;
	bool transfer_pitch_bend = true;
	bool transfer_cc = true;
	bool transfer_prog_change = true;
	bool transfer_other = true;
};

class MidiCube {
private:
	AudioHandler audio_handler;
	SoundEngineDevice engine{"Sound Engine"};
	std::array<ChannelSource, SOUND_ENGINE_MIDI_CHANNELS> channels;
	std::vector<MidiCubeInput> inputs;
public:
	MidiCube();
	void init();
	void process(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);
	void midi_callback(MidiMessage& message, size_t input);
	~MidiCube();
};


#endif /* MIDICUBE_MIDICUBE_H_ */
