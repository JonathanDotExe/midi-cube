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

#define FM_OPERATOR_COUNT 8

class FMOperator {
public:
	double volume;
	ADSREnvelope env{0.0, 0, 1, 0.0};
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY, 8> osc;

};

class FMSynth : public BaseSoundEngine {

private:
	std::array<FMOperator, FM_OPERATOR_COUNT> operators;
public:

	FMSynth();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);

	~FMSynth();

};


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
