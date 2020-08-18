/*
 * organ.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_ORGAN_H_
#define MIDICUBE_SOUNDENGINE_ORGAN_H_


#include "soundengine.h"
#include "../synthesis.h"

#define ORGAN_DRAWBAR_COUNT 9
#define ORGAN_DRAWBAR_MAX 8
#define ROTARY_CUTOFF 800
#define ROTARY_HORN_SLOW_FREQUENCY 0.8
#define ROTARY_HORN_FAST_FREQUENCY 6.8
#define ROTARY_BASS_SLOW_FREQUENCY 0.76
#define ROTARY_BASS_FAST_FREQUENCY 6.5
#define SPEAKER_RADIUS 0.25
#define HORN_RADIUS 0.15
#define BASS_RADIUS 0.15
#define SOUND_SPEED 343.2
#define ORGAN_REP_DELAY 0.000728226

struct B3OrganPreset {
	std::array<int, ORGAN_DRAWBAR_COUNT> drawbars = {8, 8, 8, 8, 8, 8, 8, 8, 8};
	std::array<unsigned int, ORGAN_DRAWBAR_COUNT> drawbar_ccs = {67, 68, 69, 70, 87, 88, 89, 90, 92};
	bool rotary = false;
	bool rotary_fast = true;
	unsigned int rotary_cc = 22;
	unsigned int rotary_speed_cc = 23;
};

class B3OrganData : public SoundEngineData{
public:
	B3OrganPreset preset;
	DelayBuffer left_horn_del;
	DelayBuffer left_bass_del;
	DelayBuffer right_horn_del;
	DelayBuffer right_bass_del;

	virtual SoundEngineData* copy() {
		return new B3OrganData();	//TODO
	}
};

class B3Organ : public SoundEngine {

private:
	B3OrganPreset data;
	std::array<double, ORGAN_DRAWBAR_COUNT> drawbar_harmonics;
public:
	B3Organ();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, SoundEngineData& data);

	void control_change(unsigned int control, unsigned int value, SoundEngineData& data);

	SoundEngineData* create_data();

	std::string get_name();

};


#endif /* MIDICUBE_SOUNDENGINE_ORGAN_H_ */
