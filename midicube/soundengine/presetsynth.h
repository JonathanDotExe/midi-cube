/*
 * presetsynth.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_
#define MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_

#include "soundengine.h"

#define SYNTH_VIBRATO_RATE 6
#define SYNTH_VIBRATO_DETUNE 1

class PresetSynth : public SoundEngine {

private:
	double detune;
	double ndetune;
	double vibrato;
	ADSREnvelope env{0.0005, 0.0, 1, 0.0005};
	LowPassFilter filter;
public:

	PresetSynth();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	std::string get_name();

};


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
