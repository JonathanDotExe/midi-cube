/*
 * vocoder.h
 *
 *  Created on: 11 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_VOCODER_H_
#define MIDICUBE_SOUNDENGINE_VOCODER_H_

#include "soundengine.h"

#define VOCODER_BAND_COUNT 40
#define VOCODER_LOW_BAND 120
#define VOCODER_HIGH_BAND 440

struct VocoderData {
	bool delay = true;
	double delay_time = 0.5;
	unsigned int delay_feedback = 4;
	double delay_mul = 1;
	double vocoder_amp = 1;
	double voice_amp = 0;
	double carrier_amp = 0;

	unsigned int delay_control = 22;
	unsigned int delay_time_control = 35;
	unsigned int delay_feedback_control = 36;
	unsigned int delay_mul_control = 37;
	unsigned int vocoder_amp_control = 38;
	unsigned int voice_amp_control = 39;
	unsigned int carrier_amp_control = 40;
};

class Vocoder : public SoundEngine {

private:
	VocoderData data;
	DelayBuffer delay;
	std::array<BandPassFilter, VOCODER_BAND_COUNT> carrier_filters;
	std::array<BandPassFilter, VOCODER_BAND_COUNT> modulator_filters;

public:
	Vocoder();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	void control_change(unsigned int control, unsigned int value);

	std::string get_name();

	~Vocoder();
};

#endif /* MIDICUBE_SOUNDENGINE_VOCODER_H_ */
