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
#include "../property.h"

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



enum DistortionType {
	DIGITAL_DISTORTION, ANALOG_DISTORTION_1, ANALOG_DISTORTION_2
};

enum B3OrganProperty {
	pB3Drawbar1,
	pB3Drawbar2,
	pB3Drawbar3,
	pB3Drawbar4,
	pB3Drawbar5,
	pB3Drawbar6,
	pB3Drawbar7,
	pB3Drawbar8,
	pB3Drawbar9,

	pB3DrawbarCC1,
	pB3DrawbarCC2,
	pB3DrawbarCC3,
	pB3DrawbarCC4,
	pB3DrawbarCC5,
	pB3DrawbarCC6,
	pB3DrawbarCC7,
	pB3DrawbarCC8,
	pB3DrawbarCC9,

	pB3HarmonicFoldbackVolume,

	pB3DistortionType,
	pB3NormalizeOverdrive,
	pB3Overdrive,
	pB3OverdriveCC,

	pB3MultiNoteGain,

	pB3Rotary,
	pB3RotarySpeed,
	pB3RotaryCC,
	pB3RotarySpeedCC,

	pB3RotaryStereoMix,
	pB3RotaryGain,
	pB3RotaryType,
	pB3RotaryDelay,

	pB3Percussion,
	pB3PercussionThirdHarmonic,
	pB3PercussionSoft,
	pB3PercussionFastDecay,

	pB3PercussionSoftVolume,
	pB3PercussionHardVolume,
	pB3PercussionFastDecayTime,
	pB3PercussionSlowDecayTime,

	pB3PercussionCC,
	pB3PercussionThirdHarmonicCC,
	pB3PercussionSoftCC,
	pB3PercussionFastDecayCC,
};

struct B3OrganPreset {
	std::array<unsigned int, ORGAN_DRAWBAR_COUNT> drawbars;
	std::array<unsigned int, ORGAN_DRAWBAR_COUNT> drawbar_ccs;
	double harmonic_foldback_volume{1.0};

	DistortionType distortion_type{DistortionType::ANALOG_DISTORTION_1};
	bool normalize_overdrive{false};
	double overdrive{0};
	unsigned int overdrive_cc{35};

	double multi_note_gain{0.75};

	bool rotary{false};
	bool rotary_fast{true};
	unsigned int rotary_cc{22};
	unsigned int rotary_speed_cc{23};

	double rotary_stereo_mix{0.5};
	double rotary_gain{1.5};
	bool rotary_type{false};
	double rotary_delay{0.0005};


	bool percussion{false};
	bool percussion_third_harmonic{true};
	bool percussion_soft{true};
	bool percussion_fast_decay{true};

	double percussion_soft_volume{0.5};
	double percussion_hard_volume{1.0};
	double percussion_fast_decay_time{0.2};
	double percussion_slow_decay_time{1.0};

	unsigned int percussion_cc{24};
	unsigned int percussion_third_harmonic_cc{25};
	unsigned int percussion_soft_cc{26};
	unsigned int percussion_fast_decay_cc{27};

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
public:
	double static_vol = 0;
	double dynamic_vol = 0;
	double curr_vol = 0;

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

class B3Organ : public BaseSoundEngine, PropertyHolder {

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

	PropertyValue get(size_t prop);

	void set(size_t prop, PropertyValue value);

};

#endif /* MIDICUBE_SOUNDENGINE_ORGAN_H_ */
