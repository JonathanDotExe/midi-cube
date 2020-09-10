/*
 * presetsynth.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_
#define MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_

#include "../filter.h"
#include "soundengine.h"
#include "../oscilator.h"

#define SYNTH_VIBRATO_RATE 6
#define SYNTH_VIBRATO_DETUNE 0.25

class PresetSynth : public SoundEngine {

private:
	double detune;
	double ndetune;
	double vibrato;
	ADSREnvelope env{0.1, 0, 1, 0.3};
	MultiChannelFilter<LowPassFilter<2>> filter;
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY, 8> osc;
public:

	PresetSynth();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& data);

	void control_change(unsigned int control, unsigned int value, SoundEngineData& data);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data);

	std::string get_name();

	~PresetSynth();

};


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
