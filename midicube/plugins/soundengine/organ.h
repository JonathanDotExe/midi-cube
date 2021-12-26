/*
 * organ.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_ORGAN_H_
#define MIDICUBE_SOUNDENGINE_ORGAN_H_

#include "../../framework/data/binding.h"
#include "../../framework/core/plugins/soundengine.h"
#include "../../framework/dsp/synthesis.h"

#define ORGAN_DRAWBAR_COUNT 9
#define ORGAN_BASS_DRAWBAR_COUNT 2
#define ORGAN_DRAWBAR_MAX 8
#define ORGAN_FOLDBACK_NOTE 114
#define ORGAN_TONEWHEEL_AMOUNT 91
#define ORGAN_LOWEST_TONEWHEEL_NOTE 24

#define ORGAN_MAX_DOWN_DELAY 0.0035
#define ORGAN_MAX_UP_DELAY 0.005

constexpr double SWELL_MIN = 0.0;
constexpr double SWELL_RANGE = 1 - SWELL_MIN;

#define ORGAN_VIBRATO_RATE 7
#define ORGAN_VIBRATO_DELAY_STAGES 9

#define B3_ORGAN_IDENTIFIER "midicube_b3_organ"

enum OrganChorusVibratoType {
	B3_NONE, B3_CHORUS_1, B3_CHORUS_2, B3_CHORUS_3, B3_VIBRATO_1, B3_VIBRATO_2, B3_VIBRATO_3
};

enum OrganType {
	ORGAN_TYPE_B3, ORGAN_TYPE_TRANSISTOR
};

struct B3OrganPreset {
	OrganType type = ORGAN_TYPE_B3;
	std::array<BindableTemplateValue<unsigned int>, ORGAN_DRAWBAR_COUNT> upper_drawbars;
	std::array<BindableTemplateValue<unsigned int>, ORGAN_DRAWBAR_COUNT> lower_drawbars;
	std::array<BindableTemplateValue<unsigned int>, ORGAN_BASS_DRAWBAR_COUNT> bass_drawbars;
	double harmonic_foldback_volume{1};
	double multi_note_gain{0.8};
	double high_gain_reduction = 0.5;
	double click_attack = 0.001;
	double click_release = 0.001;

	BindableBooleanValue percussion{false};
	BindableBooleanValue percussion_third_harmonic{true};
	BindableBooleanValue percussion_soft{true};
	BindableBooleanValue percussion_fast_decay{true};

	double percussion_soft_volume{2};
	double percussion_hard_volume{5};
	double percussion_fast_decay_time{0.2};
	double percussion_slow_decay_time{1.0};

	BindableTemplateValue<double> vibrato_mix{0, 0, 1};
	BindableTemplateValue<double> swell{1, 0, 1};

	B3OrganPreset () : upper_drawbars{
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
	}, lower_drawbars{
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
	},
	bass_drawbars{
		BindableTemplateValue<unsigned int>(8, 0, 8),
		BindableTemplateValue<unsigned int>(8, 0, 8),
	} {
		swell.type = ControlType::EXPRESSION_PEDAL;
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
	double compress_volume = 0;
	PortamendoBuffer volume_buffer{0};
	double process(const SampleInfo& info, double freq, OrganType type, double vol_mul, double click_attack, double click_release);
};

class B3OrganData {
public:
	B3OrganPreset preset;
	Filter swell_filter;
	std::array<B3OrganTonewheel, ORGAN_TONEWHEEL_AMOUNT> tonewheels = {};
	std::array<DelayBuffer, ORGAN_VIBRATO_DELAY_STAGES> delays;

	bool reset_percussion = true;
	double percussion_start = 0;
	double scanner_phase = 0;
	bool scanner_inverse = false;
};

class B3OrganProgram : public PluginProgram {
public:
	B3OrganPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();
};

#define B3_ORGAN_POLYPHONY 61

class B3Organ : public SoundEngine<TriggeredNote, B3_ORGAN_POLYPHONY> {

private:
	//Static values
	std::array<int, ORGAN_DRAWBAR_COUNT> drawbar_notes;
	std::array<B3OrganTonewheelData, ORGAN_TONEWHEEL_AMOUNT> tonewheel_data;

	void trigger_tonewheel(int tonewheel, double vol, const SampleInfo& info, TriggeredNote& note, double compress_volume);

	LocalMidiBindingHandler binder;

public:
	B3OrganData data;

	B3Organ(PluginHost& h, Plugin& p);

	void process_note_sample(const SampleInfo& info, TriggeredNote& note, size_t note_index);

	void process_sample(const SampleInfo& info);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(const SampleInfo& info, TriggeredNote& note, size_t note_index);

	void save_program(PluginProgram **prog);

	void apply_program(PluginProgram *prog);

	Menu* create_menu();

	Menu* create_menu(unsigned int channel);

	~B3Organ() {

	}

};

class OrganPlugin : public Plugin {
public:
	OrganPlugin() : Plugin({
		"B3 Organ",
		B3_ORGAN_IDENTIFIER,
		PluginType::PLUGIN_TYPE_SOUND_ENGINE,
		0,
		2,
		true,
		false
	}){

	}

	virtual PluginProgram* create_program();
	virtual PluginInstance* create(PluginHost *host);

	virtual ~OrganPlugin() {

	}
};

#endif /* MIDICUBE_SOUNDENGINE_ORGAN_H_ */
