/*
 * midi-cube.h
 *
 *  Created on: 17 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_MULTIMIDICUBE_H_
#define MIDICUBE_MULTIMIDICUBE_H_

#include <unordered_map>
#include <vector>

#include "audio.h"
#include "device.h"
#include "soundengine/engines.h"

class MultiMidiCube;

struct MultiCallbackUserData {
	MultiMidiCube* cube;
	std::string device;
};

struct DeviceBinding {
	std::string input;
	std::string output;
	int input_channel;
	int output_channel;
	unsigned int start_note = 0;
	unsigned int end_note = 127;
	int octave = 0;
	bool transfer_channel_aftertouch = true;
	bool transfer_pitch_bend = true;
	bool transfer_cc = true;
	bool transfer_prog_change = true;
	bool transfer_other = true;
};

class MultiMidiCube {
private:
	AudioHandler audio_handler;
	std::unordered_map<std::string, AudioDevice*> devices;
	std::unordered_map<std::string, std::vector<DeviceBinding>> bindings;
	std::vector<MultiCallbackUserData*> callback_data;
public:
	MultiMidiCube();
	~MultiMidiCube();
	void init();
	std::unordered_map<std::string, AudioDevice*> get_devices();
	std::vector<DeviceBinding> get_bindings();
	void create_default_devices();
	void add_device(AudioDevice* device);
	void process(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);
	void midi_callback(MidiMessage& message, std::string device);
	void add_binding(DeviceBinding binding);
};

#endif /* MIDICUBE_MULTIMIDICUBE_H_ */
