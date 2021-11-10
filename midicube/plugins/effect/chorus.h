/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_CHORUS_H_
#define MIDICUBE_EFFECT_CHORUS_H_


#include "../../framework/core/audio.h"
#include "../../framework/dsp/oscilator.h"
#include "../../framework/core/plugins/effect.h"


#define CHORUS_IDENTIFIER "midicube_chorus"

struct ChorusPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> vibrato_rate{2, 0, 8};
	BindableTemplateValue<double> vibrato_depth{0.5, 0, 1};
	BindableTemplateValue<double> mix{0.5, 0, 1};

	BindableTemplateValue<double> delay{0.015, 0, 0.03};
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class ChorusProgram : public PluginProgram {
public:
	ChorusPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~ChorusProgram() {

	}
};

class ChorusEffect : public Effect {
private:
	AnalogOscilator osc;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

public:
	ChorusPreset preset;

	ChorusEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	ViewController* create_view();
	~ChorusEffect();
};

class ChorusPlugin : public EffectPlugin<ChorusEffect, ChorusProgram> {
public:
	ChorusPlugin() : EffectPlugin({
		"Chorus",
		CHORUS_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_CHORUS_H_ */
