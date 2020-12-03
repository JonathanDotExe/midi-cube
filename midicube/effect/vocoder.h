/*
 * vocoder.h
 *
 *  Created on: Dec 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_VOCODER_H_
#define MIDICUBE_EFFECT_VOCODER_H_

#include <array>

#include <../synthesis.h>
#include <../envelope.h>
#include <../filter.h>

#define VOCODER_BAND_COUNT 40
#define VOCODER_LOW_BAND 120
#define VOCODER_HIGH_BAND 440

struct VocoderPreset {
	bool on = false;
	double vocoder_amp = 1;
	double voice_amp = 0;
	double carrier_amp = 0;
	double gate = 0.00;
};

struct VocoderBand {
	FilterData filter_data;
	Filter lfilter;
	Filter rfilter;
	Filter mfilter;
	EnvelopeFollower env;
};

class VocoderEffect {
private:
	std::array<EnvelopeFollower, VOCODER_BAND_COUNT> bands;
	EnvelopeFollower modulator_env;
public:
	VocoderEffect();

	void apply(double& lsample, double& rsample, double modulator, VocoderPreset& preset, SampleInfo& info);

};



#endif /* MIDICUBE_EFFECT_VOCODER_H_ */
