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
#define ASYNTH_MOTION_SEQUENCER_LENGTH 16

struct BindableADSREnvelopeData {
	BindableTemplateValue<double> attack{0.0005, 0.0005, 5};
	BindableTemplateValue<double> decay{0, 0, 10};
	BindableTemplateValue<double> sustain{1, 0, 1};
	BindableTemplateValue<double> release{0.0005, 0.0005, 10};

	ADSREnvelopeShape attack_shape = ADSREnvelopeShape::LINEAR_ADSR;
	ADSREnvelopeShape pre_decay_shape = ADSREnvelopeShape::ANALOG_ADSR;
	ADSREnvelopeShape decay_shape = ADSREnvelopeShape::ANALOG_ADSR;
	ADSREnvelopeShape release_shape = ADSREnvelopeShape::ANALOG_ADSR;

	BindableTemplateValue<double> hold{0, 0, 10};
	BindableTemplateValue<double> pre_decay{0, 0, 10};

	BindableTemplateValue<double> attack_hold{0, 0, 5};
	BindableTemplateValue<double> peak_volume{1, 0, 1};
	BindableTemplateValue<double> decay_volume{1, 0, 1};
	BindableTemplateValue<double> sustain_time{0, 0, 20};
	BindableTemplateValue<double> release_volume{0, 0, 1};

	bool pedal_catch = false;

	BindableADSREnvelopeData(double attack = 0.0005, double decay = 0, double sustain = 1, double release = 0.0005) {
		this->attack = attack;
		this->sustain = sustain;
		this->decay = decay;
		this->release = release;
	}


	operator ADSREnvelopeData() const {
		ADSREnvelopeData env;
		env.attack = attack.get_temp();
		env.decay = decay.get_temp();
		env.sustain = sustain.get_temp();
		env.release = release.get_temp();

		env.attack_shape = attack_shape;
		env.pre_decay_shape = pre_decay_shape;
		env.decay_shape = decay_shape;
		env.release_shape = release_shape;

		env.hold = hold.get_temp();
		env.pre_decay =  pre_decay.get_temp();

		env.attack_hold = attack_hold.get_temp();
		env.peak_volume = peak_volume.get_temp();
		env.decay_volume = decay_volume.get_temp();
		env.sustain_time = sustain_time.get_temp();
		env.release_volume = release_volume.get_temp();
		return env;
	}
};

struct PropertyModulation {
	BindableTemplateValue<double> value{0, 0, 1};
	size_t mod_env = 0;
	double mod_env_amount = 0;
	size_t lfo = 0;
	double lfo_amount = 0;
	double velocity_amount = 0;
	double aftertouch_amount = 0;

	PropertyModulation(double val) {
		value = val;
	}
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
	double phase = 0;

	size_t unison_amount = 0;
	PropertyModulation volume = {1};
	PropertyModulation sync_mul = {0.0};
	PropertyModulation pulse_width = {1.0};
	PropertyModulation unison_detune = {0.1};
	int semi = 0;
	double transpose = 1;
	PropertyModulation pitch = {0.5};

	bool fixed_freq = false;
	unsigned int note = 60;
};

struct FilterEntity {
	bool on = false;
	bool drive = false;
	double drive_amount = 0;
	FilterType type = FilterType::LP_12;
	PropertyModulation cutoff = {1};
	PropertyModulation resonance = {0};
	BindableTemplateValue<double> kb_track{0, 0, 1};
	unsigned int kb_track_note = 36;
};

struct OperatorEntity {
	bool audible = true;
	BindableADSREnvelopeData env{0.0005, 0, 1, 0.0005};
	double amp_kb_track_upper = 0;
	double amp_kb_track_lower = 0;
	unsigned int amp_kb_track_note = 60;

	PropertyModulation volume = {1};
	PropertyModulation panning = {0.5};

	FilterEntity first_filter;
	FilterEntity second_filter;
	bool filter_parallel = false;

	unsigned int oscilator_count = 1;
	std::array<double, ANALOG_PART_COUNT> fm;

};


struct ModEnvelopeEntity {
	PropertyModulation volume = {1};
	BindableADSREnvelopeData env{0.0005, 0, 1, 0.0005};
};

struct LFOEntity {
	PropertyModulation volume = {1};
	BindableTemplateValue<double> freq{1, 0, 50};
	bool sync_master = false;
	int clock_value = 1;
	double sync_phase = 0;
	AnalogWaveForm waveform = AnalogWaveForm::SINE_WAVE;
	int motion_sequencer = -1;
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
	BindableTemplateValue<double> portamendo{0, 0, 10};

	double aftertouch_attack = 0;
	double aftertouch_release = 0;
	bool max_aftertouch = false;
	double velocity_aftertouch_amount = 0;
};

struct AnalogSynthPart {
	UnisonOscilator<8> oscilator;
	Filter filter;
	WaveTableADSREnvelope amp_env;
	WaveTableADSREnvelope mod_env;
	Filter filter1;
	Filter filter2;

	double fm = 0;
};

struct AnalogSynthVoice : public TriggeredNote {
	std::array<AnalogSynthPart, ANALOG_PART_COUNT> parts;
	double max_aftertouch = 0;
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
	std::array<double, ANALOG_PART_COUNT> lfo_vol = {};
	std::array<double, ANALOG_CONTROL_COUNT> controls;
	PortamendoBuffer aftertouch{0, 0};

	bool first_port = true;
	PortamendoBuffer note_port{0, 0};
	AnalogSynthVoice mono_voice;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

	inline void process_note(double& lsample, double& rsample, SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env);

	inline bool amp_finished(SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env);

	inline void apply_filter(FilterEntity filter, Filter& f, double& carrier, AnalogSynthVoice &note, double time_step, double velocity, double aftertouch);

	inline double apply_modulation(const FixedScale &scale, PropertyModulation &mod, double velocity, double aftertouch, std::array<double, ANALOG_PART_COUNT>& lfo_val);

	LocalMidiBindingHandler binder;

public:
	AnalogSynthPreset preset;

	AnalogSynth();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	bool midi_message(MidiMessage& msg, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit);

	bool control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, AnalogSynthVoice& note, KeyboardEnvironment& env, size_t note_index);
	
	void press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit);

	void release_note(SampleInfo& info, unsigned int note);

	void save_program(EngineProgram **prog);

	void apply_program(EngineProgram *prog);

	~AnalogSynth();

};


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
