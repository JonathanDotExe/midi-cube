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

#define SYNTH_VIBRATO_RATE 6
#define SYNTH_VIBRATO_DETUNE 0.25

class PresetSynth : public SoundEngine {

private:
	double detune;
	double ndetune;
	double vibrato;
	ADSREnvelope env{0.0005, 0.5, 0, 0.0005};
	MultiChannelFilter<LowPassFilter> filter;
	OscilatorSlot* osc;
	OscilatorSlot* osc2;
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
