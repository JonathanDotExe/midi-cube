/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_PHASER_H_
#define MIDICUBE_EFFECT_PHASER_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "../framework/core/plugins/effect.h"

#define PHASER_ALLPASS_AMOUNT 4
#define PHASER_IDENTIFIER "midicube_phaser"

struct PhaserPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> center_cutoff{0.5, 0, 1};
	BindableTemplateValue<double> lfo_depth{0.5, 0, 1};
	BindableTemplateValue<double> lfo_rate{1, 0, 8};
	BindableTemplateValue<double> mix{0.5, 0, 1};
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class PhaserProgram : public PluginProgram {
public:
	PhaserPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~PhaserProgram() {

	}
};

class PhaserEffect : public Effect {
private:
	AnalogOscilator osc;
	std::array<Filter, PHASER_ALLPASS_AMOUNT> lfilter;
	std::array<Filter, PHASER_ALLPASS_AMOUNT> rfilter;

public:
	PhaserPreset preset;

	PhaserEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	~PhaserEffect();
};

class PhaserPlugin : public EffectPlugin<PhaserEffect, PhaserProgram> {
public:
	PhaserPlugin() : EffectPlugin({
		"Phaser",
		PHASER_IDENTIFIER,
		PluginType::EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};


#endif /* MIDICUBE_EFFECT_PHASER_H_ */
