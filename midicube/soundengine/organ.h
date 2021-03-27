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
#include "../effect/rotary_speaker.h"
#include "../effect/amplifier_simulation.h"
#include "../property.h"

#define ORGAN_DRAWBAR_COUNT 9
#define ORGAN_DRAWBAR_MAX 8
#define ORGAN_FOLDBACK_NOTE 114
#define ORGAN_TONEWHEEL_AMOUNT 91
#define ORGAN_LOWEST_TONEWHEEL_NOTE 24

#define ORGAN_MAX_DOWN_DELAY 0.0035
#define ORGAN_MAX_UP_DELAY 0.0025

#define MIN_SWELL 0.1
#define SWELL_RANGE (1 - MIN_SWELL)

#define ORGAN_VIBRATO_RATE 7
#define ORGAN_VIBRATO_DELAY_STAGES 9


enum B3OrganProperty {
	pB3OrganType,

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

	pB3AmpOn,
	pB3AmpDrive,
	pB3AmpBoost,
	pB3AmpTone,
	pB3AmpDistortionType,
	pB3AmpOnCC,
	pB3AmpBoostCC,
	pB3AmpDriveCC,
	pB3AmpToneCC,

	pB3MultiNoteGain,

	pB3Rotary,
	pB3RotarySpeed,
	pB3RotaryCC,
	pB3RotarySpeedCC,

	pB3RotaryStereoMix,
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

	pB3VibratoMix,
	pB3VibratoMixCC,
	pB3SwellCC,
};

enum OrganChorusVibratoType {
	B3_NONE, B3_CHORUS_1, B3_CHORUS_2, B3_CHORUS_3, B3_VIBRATO_1, B3_VIBRATO_2, B3_VIBRATO_3
};

enum OrganType {
	ORGAN_TYPE_B3, ORGAN_TYPE_TRANSISTOR
};

struct B3OrganPreset {
	OrganType type = ORGAN_TYPE_B3;
	std::array<unsigned int, ORGAN_DRAWBAR_COUNT> drawbars;
	std::array<unsigned int, ORGAN_DRAWBAR_COUNT> drawbar_ccs;
	double harmonic_foldback_volume{1};
	double multi_note_gain{0.8};
	double click_attack = 0.00025;

	AmplifierSimulationPreset amplifier;
	unsigned int amp_cc{28};
	unsigned int amp_boost_cc{35};
	unsigned int amp_drive_cc{36};
	unsigned int amp_tone_cc{37};

	RotarySpeakerPreset rotary;
	unsigned int rotary_cc{22};
	unsigned int rotary_speed_cc{23};

	bool percussion{false};
	bool percussion_third_harmonic{true};
	bool percussion_soft{true};
	bool percussion_fast_decay{true};

	double percussion_soft_volume{1};
	double percussion_hard_volume{5};
	double percussion_fast_decay_time{0.2};
	double percussion_slow_decay_time{1.0};

	unsigned int percussion_cc{24};
	unsigned int percussion_third_harmonic_cc{25};
	unsigned int percussion_soft_cc{26};
	unsigned int percussion_fast_decay_cc{27};

	double vibrato_mix = 0;
	unsigned int vibrato_mix_cc{38};
	unsigned int swell_cc{11};

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

struct B3OrganTonewheelData {
	double freq;
	double press_delay;
	double release_delay;
	double volume = 1;
};

class B3OrganTonewheel {
public:
	double rotation = 0;
	double volume = 0;
	double process(SampleInfo& info, double freq, OrganType type);
};

class B3OrganData {
public:
	B3OrganPreset preset;
	AmplifierSimulationEffect amplifier;
	RotarySpeakerEffect rotary_speaker;
	std::array<B3OrganTonewheel, ORGAN_TONEWHEEL_AMOUNT> tonewheels = {};
	std::array<DelayBuffer, ORGAN_VIBRATO_DELAY_STAGES> delays;

	bool reset_percussion = true;
	double percussion_start = 0;
	double scanner_phase = 0;
	bool scanner_inverse = false;

	double swell = 1;
};

class B3OrganProgram : public EngineProgram {
public:
	B3OrganPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();
};

#define B3_ORGAN_POLYPHONY 61

class B3Organ : public BaseSoundEngine<TriggeredNote, B3_ORGAN_POLYPHONY> , public PropertyHolder {

private:
	//Static values
	std::array<int, ORGAN_DRAWBAR_COUNT> drawbar_notes;
	std::array<B3OrganTonewheelData, ORGAN_TONEWHEEL_AMOUNT> tonewheel_data;

	void trigger_tonewheel(int tonewheel, double vol, SampleInfo& info, TriggeredNote& note);

public:
	B3OrganData data;

	B3Organ();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void update_properties();

	PropertyValue get(size_t prop, size_t sub_prop = 0);

	void set(size_t prop, PropertyValue value, size_t sub_prop = 0);

	void save_program(EngineProgram **prog);

	void apply_program(EngineProgram *prog);

};

#endif /* MIDICUBE_SOUNDENGINE_ORGAN_H_ */
