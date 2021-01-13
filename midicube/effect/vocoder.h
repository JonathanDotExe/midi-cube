/*
 * vocoder.h
 *
 *  Created on: Dec 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_VOCODER_H_
#define MIDICUBE_EFFECT_VOCODER_H_

#include <array>

#include "../synthesis.h"
#include "../envelope.h"
#include "../filter.h"

#define VOCODER_BAND_COUNT 8
#define VOCODER_LOW_BAND 120
#define VOCODER_HIGH_BAND 160

struct VocoderPreset {
	bool on = false;
	double vocoder_amp = 0.4;
	double voice_amp = 0.6;
	double carrier_amp = 0;
	double gate = 0;
	double mod_highpass = 1500;
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
	std::array<VocoderBand, VOCODER_BAND_COUNT> bands;
	Filter mfilter;
	EnvelopeFollower modulator_env;
public:
	VocoderEffect();

	void apply(double& lsample, double& rsample, double modulator, VocoderPreset& preset, SampleInfo& info);

};



#endif /* MIDICUBE_EFFECT_VOCODER_H_ */
