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
#define ORGAN_FOLDBACK_NOTE 114
#define ORGAN_TONEWHEEL_AMOUNT 91
#define ORGAN_LOWEST_TONEWHEEL_NOTE 24

#define ROTARY_CUTOFF 800

#define ROTARY_HORN_SLOW_FREQUENCY 0.83333333333333
#define ROTARY_HORN_FAST_FREQUENCY 6.66666666666667
#define ROTARY_BASS_SLOW_FREQUENCY 0.66666666666667
#define ROTARY_BASS_FAST_FREQUENCY 5.66666666666667

#define ROTARY_HORN_SLOW_RAMP 1.6
#define ROTARY_HORN_FAST_RAMP 1.0
#define ROTARY_BASS_SLOW_RAMP 5.5
#define ROTARY_BASS_FAST_RAMP 5.5

#define HORN_RADIUS 0.15
#define BASS_RADIUS 0.15
#define SOUND_SPEED 343.2

enum class DisctortionType {
	DIGITAL, ANALOG_1
};

struct B3OrganPreset {
	std::array<std::atomic<unsigned int>, ORGAN_DRAWBAR_COUNT> drawbars;
	std::array<std::atomic<unsigned int>, ORGAN_DRAWBAR_COUNT> drawbar_ccs;
	std::atomic<double> harmonic_foldback_volume{1.0};

	std::atomic<DisctortionType> distortion_type{DisctortionType::ANALOG_1};
	std::atomic<double> overdrive{false};
	std::atomic<unsigned int> overdrive_cc{35};

	std::atomic<bool> rotary{false};
	std::atomic<bool> rotary_fast{true};
	std::atomic<unsigned int> rotary_cc{22};
	std::atomic<unsigned int> rotary_speed_cc{23};

	std::atomic<double> rotary_stereo_mix{0.5};
	std::atomic<double> rotary_gain{1.5};
	std::atomic<bool> rotary_type{false};
	std::atomic<double> rotary_delay{0.0005};


	std::atomic<bool> percussion{false};
	std::atomic<bool> percussion_third_harmonic{true};
	std::atomic<bool> percussion_soft{true};
	std::atomic<bool> percussion_fast_decay{true};

	std::atomic<double> percussion_soft_volume{0.5};
	std::atomic<double> percussion_hard_volume{1.0};
	std::atomic<double> percussion_fast_decay_time{0.2};
	std::atomic<double> percussion_slow_decay_time{1.0};

	std::atomic<unsigned int> percussion_cc{24};
	std::atomic<unsigned int> percussion_third_harmonic_cc{25};
	std::atomic<unsigned int> percussion_soft_cc{26};
	std::atomic<unsigned int> percussion_fast_decay_cc{27};

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

class B3OrganTonewheel {
private:
	double rotation = 0;
	double last_turn = 0;
	double curr_vol = 0;
public:
	double static_vol = 0;
	double dynamic_vol = 0;

	bool has_turned_since(double time);
	double process(SampleInfo& info, double freq);
};

class B3OrganData : public SoundEngineData{
public:
	B3OrganPreset preset;
	std::array<B3OrganTonewheel, ORGAN_TONEWHEEL_AMOUNT> tonewheels;

	DelayBuffer left_horn_del;
	DelayBuffer right_horn_del;
	DelayBuffer left_bass_del;
	DelayBuffer right_bass_del;
	bool curr_rotary_fast = 0;
	PortamendoBuffer horn_speed{ROTARY_HORN_SLOW_FREQUENCY, ROTARY_HORN_SLOW_RAMP};
	PortamendoBuffer bass_speed{ROTARY_BASS_SLOW_FREQUENCY, ROTARY_BASS_SLOW_RAMP};
	double horn_rotation = 0;
	double bass_rotation = 0;

	bool reset_percussion = true;
	double percussion_start = 0;

	virtual SoundEngineData* copy() {
		return new B3OrganData();	//TODO
	}
};

class B3Organ : public BaseSoundEngine {

private:
	//Static values
	std::array<double, ORGAN_DRAWBAR_COUNT> drawbar_harmonics;
	std::array<int, ORGAN_DRAWBAR_COUNT> drawbar_notes;
	std::array<double, ORGAN_TONEWHEEL_AMOUNT> tonewheel_frequencies;
	size_t cutoff_tonewheel = 0;
	double foldback_freq = 0;

public:
	B3OrganData data;

	B3Organ();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	void control_change(unsigned int control, unsigned int value);

};

#endif /* MIDICUBE_SOUNDENGINE_ORGAN_H_ */
