/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_FLANGER_H_
#define MIDICUBE_EFFECT_FLANGER_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "../framework/core/plugins/effect.h"

#define FLANGER_IDENTIFIER "midicube_flanger"

struct FlangerPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> vibrato_rate{2, 0, 8};
	BindableTemplateValue<double> vibrato_depth{0.5, 0, 1};
	BindableTemplateValue<double> mix{0.5, 0, 1};

	BindableTemplateValue<double> delay{0.005, 0, 0.02};
	BindableTemplateValue<double> feedback{0.0, 0, 1.0};
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class FlangerProgram : public PluginProgram {
public:
	FlangerPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~FlangerProgram() {

	}
};

class FlangerEffect : public Effect {
private:
	AnalogOscilator osc;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

public:
	FlangerPreset preset;

	FlangerEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	~FlangerEffect();
};

class FlangerPlugin : public EffectPlugin<FlangerEffect, FlangerProgram> {
public:
	FlangerPlugin() : EffectPlugin({
		"Flanger",
		FLANGER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};


#endif /* MIDICUBE_EFFECT_FLANGER_H_ */
