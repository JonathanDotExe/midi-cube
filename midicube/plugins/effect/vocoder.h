/*
 * vocoder.h
 *
 *  Created on: Dec 3, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_VOCODER_H_
#define MIDICUBE_EFFECT_VOCODER_H_

#include <array>

#include "../../framework/dsp/synthesis.h"
#include "../../framework/dsp/envelope.h"
#include "../../framework/dsp/filter.h"
#include "../../framework/core/plugins/effect.h"

#define VOCODER_BAND_COUNT 32
#define VOCODER_IDENTIFIER "midicube_vocoder"

const std::vector<double> frequencies = {
		240, 2400,
		235, 2100,
		390, 2300,
		370, 1900,
		610, 1900,
		585, 1710,
		850, 1610,
		820, 1530,
		750, 940,
		700, 760,
		600, 1170,
		500, 700,
		460, 1310,
		360, 640,
		300, 1390,
		250, 595
};

struct VocoderPreset {
	bool on = true;
	double modulator_amplification = 5;
	double post_amplification = 10;
	double modulator_mix = 0.2;
	double mix = 0;

	double gate = 0;
	double mod_highpass = 1200;

	double normalization = 0.2;
	double slope = 0.0;

	bool formant_mode = true;
	double min_freq = 120;
	double max_freq = 360;
	double resonance = 0;
	FilterType filter_type = FilterType::LP_12_BP;
};

struct VocoderBand {
	Filter lfilter;
	Filter rfilter;
	Filter mfilter;
	EnvelopeFollower env{};
	PortamendoBuffer port{0, 0};
};

class VocoderProgram : public PluginProgram {
public:
	VocoderPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~VocoderProgram() {

	}
};

class VocoderEffect : public Effect {
private:
	std::array<VocoderBand, VOCODER_BAND_COUNT> bands;
	Filter mfilter;
	EnvelopeFollower modulator_env{};
public:
	VocoderPreset preset;

	VocoderEffect(PluginHost& h, Plugin& p) : Effect(h, p) {

	}
	Menu* create_menu();
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	~VocoderEffect() {

	}

	void process(const SampleInfo& info);

};

class VocoderPlugin : public EffectPlugin<VocoderEffect, VocoderProgram> {
public:
	VocoderPlugin() : EffectPlugin({
		"Vocoder",
		VOCODER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		3,
		2,
		false,
		false
	}) {

	}
};



#endif /* MIDICUBE_EFFECT_VOCODER_H_ */
