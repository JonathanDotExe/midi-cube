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
#include "../property.h"

#define ANALOG_OSCILATOR_COUNT 8
#define ANALOG_MOD_ENV_COUNT 8
#define ANALOG_LFO_COUNT 8
#define ANALOG_CONTROL_COUNT 128

const FixedScale VOLUME_SCALE = {0, {}, 1};
const FixedScale SYNC_SCALE = {0, {}, 1};
const FixedScale PULSE_WIDTH_SCALE = {0, {}, 0.5};
const FixedScale UNISON_DETUNE_SCALE = {0, {}, 1};
const FixedScale FILTER_CUTOFF_SCALE(0, {}, 1);
const FixedScale FILTER_RESONANCE_SCALE(0, {}, 1);
const FixedScale PITCH_SCALE(-2, {}, 2);
const FixedScale PANNING_SCALE(-1, {}, 1);

struct PropertyModulation {
	double value = 0;
	size_t mod_env = 0;
	double mod_amount = 0;
	size_t lfo = 0;
	double lfo_amount = 0;
	double velocity_amount = 0;
	size_t cc = 1;
	double cc_amount = 0;
};

struct FrequencyModulatotion {
	double fm_amount = 0;
	size_t modulator = 0;
};

struct OscilatorEntity {
	bool active = false;
	bool audible = true;
	ADSREnvelopeData env{0.0, 0, 1, 0.0};
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN;
	bool analog = true;
	bool sync = false;
	bool reset = false;
	bool randomize = false;

	size_t unison_amount = 0;
	PropertyModulation volume = {1};
	PropertyModulation sync_mul = {0.1};
	PropertyModulation pulse_width = {1};
	PropertyModulation unison_detune = {0.1};
	PropertyModulation panning = {0.5};
	double semi = 0;
	double transpose = 1;
	PropertyModulation pitch = {0.5};

	bool filter = false;
	FilterType filter_type = FilterType::LP_12;
	PropertyModulation filter_cutoff = {1};
	PropertyModulation filter_resonance = {0};
	double filter_kb_track = 0;
	unsigned int filter_kb_track_note = 36;

	std::vector<FrequencyModulatotion> fm;
};

struct ModEnvelopeEntity {
	bool active = false;
	PropertyModulation volume = {1};
	ADSREnvelopeData env{0.0, 0, 1, 0.0};
};

struct LFOEntity {
	bool active = false;
	PropertyModulation volume = {1};
	double freq = 1;
	AnalogWaveForm waveform = AnalogWaveForm::SINE;
};

struct AnalogSynthPreset {
	std::array<LFOEntity, ANALOG_LFO_COUNT> lfos;
	std::array<ModEnvelopeEntity, ANALOG_MOD_ENV_COUNT> mod_envs;
	std::array<OscilatorEntity, ANALOG_OSCILATOR_COUNT> oscilators;

	bool mono = false;
	bool legato = false;
	double portamendo = 0;

	//Delay effect
	double delay_time = 0;
	double delay_feedback = 0;
	double delay_mix = 0;
};
/*
	bool active = false;
	bool audible = true;
	ADSREnvelopeData env{0.0, 0, 1, 0.0};
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN;
	bool analog = true;
	bool sync = false;
	bool reset = false;
	bool randomize = false;

	size_t unison_amount = 0;
	PropertyModulation volume = {1};
	PropertyModulation sync_mul = {0.1};
	PropertyModulation pulse_width = {1};
	PropertyModulation unison_detune = {0.1};
	PropertyModulation panning = {0.5};
	double semi = 0;
	double transpose = 1;
	PropertyModulation pitch = {0.5};

	bool filter = false;
	FilterType filter_type = FilterType::LP_12;
	PropertyModulation filter_cutoff = {1};
	PropertyModulation filter_resonance = {0};
	double filter_kb_track = 0;
	unsigned int filter_kb_track_note = 36;
 */
enum SynthPartProperty {
	pSynthOscActive,
	pSynthOscAudible,
	pSynthOscAmpAttack,
	pSynthOscDecay,
	pSynthOscSustain,
	pSynthOscRelease,
	pSynthOscWaveForm,
	pSynthOscAnalog,
	pSynthOscSync,
	pSynthOscReset,
	pSynthOscRandomize,

	pSynthOscUnisonAmount,

	pSynthOscVolume,
	pSynthOscVolumeModEnv,
	pSynthOscVolumeModEnvAmount,
	pSynthOscVolumeLFO,
	pSynthOscVolumeLFOAmount,
	pSynthOscVolumeVelocityAmount,
	pSynthOscVolumeCC,
	pSynthOscVolumeCCAmount,

	pSynthOscSyncMul,
	pSynthOscSyncMulModEnv,
	pSynthOscSyncMulModEnvAmount,
	pSynthOscSyncMulLFO,
	pSynthOscSyncMulLFOAmount,
	pSynthOscSyncMulVelocityAmount,
	pSynthOscSyncMulCC,
	pSynthOscSyncMulCCAmount,

	pSynthOscPulseWidth,
	pSynthOscPulseWidthModEnv,
	pSynthOscPulseWidthModEnvAmount,
	pSynthOscPulseWidthLFO,
	pSynthOscPulseWidthLFOAmount,
	pSynthOscPulseWidthVelocityAmount,
	pSynthOscPulseWidthCC,
	pSynthOscPulseWidthCCAmount,

	pSynthOscUnisonDetune,
	pSynthOscUnisonDetuneModEnv,
	pSynthOscUnisonDetuneModEnvAmount,
	pSynthOscUnisonDetuneLFO,
	pSynthOscUnisonDetuneLFOAmount,
	pSynthOscUnisonDetuneVelocityAmount,
	pSynthOscUnisonDetuneCC,
	pSynthOscUnisonDetuneCCAmount,

	pSynthOscPulseWidth,
	pSynthOscPulseWidthModEnv,
	pSynthOscPulseWidthModEnvAmount,
	pSynthOscPulseWidthLFO,
	pSynthOscPulseWidthLFOAmount,
	pSynthOscPulseWidthVelocityAmount,
	pSynthOscPulseWidthCC,
	pSynthOscPulseWidthCCAmount,

	pSynthOscSemi,
	pSynthOscTranspose,

	pSynthOscPitch,
	pSynthOscPitchModEnv,
	pSynthOscPitchModEnvAmount,
	pSynthOscPitchLFO,
	pSynthOscPitchLFOAmount,
	pSynthOscPitchVelocityAmount,
	pSynthOscPitchCC,
	pSynthOscPitchCCAmount,
};
/*
	double value = 0;
	size_t mod_env = 0;
	double mod_amount = 0;
	size_t lfo = 0;
	double lfo_amount = 0;
	double velocity_amount = 0;
	size_t cc = 1;
	double cc_amount = 0;
 */

class SynthPartPropertyHolder : public PropertyHolder {
private:
	AnalogSynthPreset& preset;
	size_t part;
public:
	SynthPartPropertyHolder(AnalogSynthPreset& p, size_t part);
	void check_update();
	PropertyValue get(size_t prop);
	void set(size_t prop, PropertyValue value);
};

class AnalogSynth : public BaseSoundEngine {

private:
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT, 8> oscilators;
	std::array<double, SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT> modulators = {};
	std::array<Filter, SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT> filters;
	std::array<ADSREnvelope, SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT> amp_envs;
	std::array<ADSREnvelope, SOUND_ENGINE_POLYPHONY * ANALOG_MOD_ENV_COUNT> mod_envs;
	std::array<double, ANALOG_MOD_ENV_COUNT> env_val = {};
	std::array<AnalogOscilator, ANALOG_LFO_COUNT> lfos;
	std::array<double, ANALOG_LFO_COUNT> lfo_val = {};
	std::array<double, ANALOG_LFO_COUNT> lfo_mod = {};
	std::array<double, ANALOG_CONTROL_COUNT> controls;

	bool first_port = true;
	unsigned int last_note = 0;
	PortamendoBuffer note_port{0, 0};
	DelayBuffer ldelay;
	DelayBuffer rdelay;

	inline void process_note(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	inline bool amp_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

public:
	AnalogSynthPreset preset;

	AnalogSynth();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

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
	FM_KALIMBA
};

void apply_preset(SynthFactoryPreset type, AnalogSynthPreset& preset);


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
