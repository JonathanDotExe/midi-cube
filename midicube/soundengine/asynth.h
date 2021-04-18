/*
 * presetsynth.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_
#define MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_

#include "soundengine.h"
#include "../oscilator.h"
#include "../filter.h"
#include "../util.h"

#define ANALOG_PART_COUNT 8
#define ANALOG_CONTROL_COUNT 128

const FixedScale VOLUME_SCALE = {0, {}, 1};
const FixedScale SYNC_SCALE = {1, {}, 10};
const FixedScale PULSE_WIDTH_SCALE = {0, {}, 0.5};
const FixedScale UNISON_DETUNE_SCALE = {0, {}, 1};
const FixedScale FILTER_CUTOFF_SCALE(0, {}, 1);
const FixedScale FILTER_RESONANCE_SCALE(0, {}, 1);
const FixedScale PITCH_SCALE(-2, {}, 2);
const FixedScale PANNING_SCALE(-1, {}, 1);

#define ANALOG_SYNTH_POLYPHONY 30



struct PropertyModulation {
	double value = 0;
	size_t mod_env = 0;
	double mod_env_amount = 0;
	size_t lfo = 0;
	double lfo_amount = 0;
	double velocity_amount = 0;
	size_t cc = 1;
	double cc_amount = 0;
	double aftertouch_amount = 0;
};

struct FrequencyModulatotion {
	double fm_amount = 0;
	size_t modulator = 0;
};

struct OscilatorEntity {
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN_WAVE;
	bool analog = true;
	bool sync = false;
	bool reset = false;
	bool randomize = false;

	size_t unison_amount = 0;
	PropertyModulation volume = {1};
	PropertyModulation sync_mul = {0.0};
	PropertyModulation pulse_width = {1.0};
	PropertyModulation unison_detune = {0.1};
	int semi = 0;
	double transpose = 1;
	PropertyModulation pitch = {0.5};
};

struct OperatorEntity {
	bool audible = true;
	ADSREnvelopeData env{0.0005, 0, 1, 0.0005};
	double amp_kb_track_upper = 0;
	double amp_kb_track_lower = 0;
	unsigned int amp_kb_track_note = 60;

	PropertyModulation volume = {1};
	PropertyModulation panning = {0.5};

	bool filter = false;
	bool pre_filter_drive = false;
	double pre_filter_drive_amount = 0;
	FilterType filter_type = FilterType::LP_12;
	PropertyModulation filter_cutoff = {1};
	PropertyModulation filter_resonance = {0};
	double filter_kb_track = 0;
	unsigned int filter_kb_track_note = 36;

	unsigned int oscilator_count = 1;
	std::array<double, ANALOG_PART_COUNT> fm;

};


struct ModEnvelopeEntity {
	PropertyModulation volume = {1};
	ADSREnvelopeData env{0.0005, 0, 1, 0.0005};
};

struct LFOEntity {
	PropertyModulation volume = {1};
	double freq = 1;
	AnalogWaveForm waveform = AnalogWaveForm::SINE_WAVE;
};

struct AnalogSynthPreset {
	std::array<LFOEntity, ANALOG_PART_COUNT> lfos;
	std::array<ModEnvelopeEntity, ANALOG_PART_COUNT> mod_envs;
	std::array<OscilatorEntity, ANALOG_PART_COUNT> oscilators;
	std::array<OperatorEntity, ANALOG_PART_COUNT> operators;

	size_t lfo_count = 0;
	size_t mod_env_count = 0;
	size_t op_count = 1;

	bool mono = false;
	bool legato = false;
	double portamendo = 0;

	//Delay effect
	double delay_time = 0;
	double delay_feedback = 0;
	double delay_mix = 0;
};

struct AnalogSynthPart {
	UnisonOscilator<8> oscilator;
	Filter filter;
	AnalogADSREnvelope amp_env;
	LinearADSREnvelope mod_env;

	double fm = 0;
};

struct AnalogSynthVoice : public TriggeredNote {
	std::array<AnalogSynthPart, ANALOG_PART_COUNT> parts;
};

class AnalogSynthProgram : public EngineProgram {
public:
	AnalogSynthPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~AnalogSynthProgram() {

	}
};

class AnalogSynth : public BaseSoundEngine<AnalogSynthVoice, ANALOG_SYNTH_POLYPHONY> {

private:
	std::array<double, ANALOG_PART_COUNT> env_val = {};
	std::array<AnalogOscilator, ANALOG_PART_COUNT> lfos;
	std::array<double, ANALOG_PART_COUNT> lfo_val = {};
	std::array<double, ANALOG_PART_COUNT> lfo_mod = {};
	std::array<double, ANALOG_CONTROL_COUNT> controls;
	double aftertouch = 0;

	bool first_port = true;
	PortamendoBuffer note_port{0, 0};
	AnalogSynthVoice mono_voice;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

	inline void process_note(double& lsample, double& rsample, SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env);

	inline bool amp_finished(SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env);

public:
	AnalogSynthPreset preset;

	AnalogSynth();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	bool midi_message(MidiMessage& msg, SampleInfo& info);

	bool control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env, size_t note_index);
	
	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

	void save_program(EngineProgram **prog);

	void apply_program(EngineProgram *prog);

	~AnalogSynth();

};

enum SynthFactoryPreset {
	SQUASHY_BASS,
	UNISON_SAWS,
	POLY_SWEEP,
	SPOOKY_SINE,
	LUSH_LEAD,
	PULSE_BASS,
	DELAY_CHORDS,
	STRONG_PAD,
	CLEAN_SAW,
	FM_BASS,
	FM_E_PIANO,
	BRASS_PAD,
	FM_KALIMBA,
	SYNTH_BRASS,
	BELL_LEAD
};

void apply_preset(SynthFactoryPreset type, AnalogSynthPreset& preset);


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
