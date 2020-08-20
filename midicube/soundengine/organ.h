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
#define ROTARY_HORN_SLOW_RAMP 1.6
#define ROTARY_HORN_FAST_RAMP 1.0
#define ROTARY_BASS_SLOW_RAMP 5.5
#define ROTARY_BASS_FAST_RAMP 5.5
#define HORN_RADIUS 0.15
#define BASS_RADIUS 0.15
#define SOUND_SPEED 343.2

struct B3OrganPreset {
	std::array<std::atomic<unsigned int>, ORGAN_DRAWBAR_COUNT> drawbars;
	std::array<std::atomic<unsigned int>, ORGAN_DRAWBAR_COUNT> drawbar_ccs;
	std::atomic<bool> rotary{false};
	std::atomic<bool> rotary_fast{true};
	std::atomic<unsigned int> rotary_cc{22};
	std::atomic<unsigned int> rotary_speed_cc{23};

	B3OrganPreset () {
		for (size_t i = 0; i < drawbars.size(); ++i) {
			drawbars[i] = 8;
		}
		size_t i = 0;
		drawbar_ccs.at(i++) = 67;
		drawbar_ccs.at(i++) = 68;
		drawbar_ccs.at(i++) = 69;
		drawbar_ccs.at(i++) = 70;
		drawbar_ccs.at(i++) = 87;
		drawbar_ccs.at(i++) = 88;
		drawbar_ccs.at(i++) = 89;
		drawbar_ccs.at(i++) = 90;
		drawbar_ccs.at(i++) = 92;
	}
};

class B3OrganData : public SoundEngineData{
public:
	B3OrganPreset preset;
	DelayBuffer left_del;
	DelayBuffer right_del;
	bool curr_rotary_fast = 0;
	PortamendoBuffer horn_speed{ROTARY_HORN_SLOW_FREQUENCY, ROTARY_HORN_SLOW_RAMP};
	PortamendoBuffer bass_speed{ROTARY_BASS_SLOW_FREQUENCY, ROTARY_BASS_SLOW_RAMP};
	double horn_rotation = 0;
	double bass_rotation = 0;

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
