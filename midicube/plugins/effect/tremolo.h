/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_TREMOLO_H_
#define MIDICUBE_EFFECT_TREMOLO_H_

#include "../../framework/core/audio.h"
#include "../../framework/dsp/oscilator.h"
#include "../../framework/core/plugins/effect.h"

#define TREMOLO_IDENTIFIER "midicube_tremolo"

struct TremoloPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> rate{2, 0, 8};
	BindableTemplateValue<double> depth{0.5, 0, 1};
	AnalogWaveForm waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class TremoloProgram : public PluginProgram {
public:
	TremoloPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~TremoloProgram() {

	}
};

class TremoloEffect : public Effect {
private:
	AnalogOscilator osc;

public:
	TremoloPreset preset;

	TremoloEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	ViewController* create_view();
	~TremoloEffect();
};

class TremoloPlugin : public EffectPlugin<TremoloEffect, TremoloProgram> {
public:
	TremoloPlugin() : EffectPlugin({
		"Tremolo",
		TREMOLO_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_TREMOLO_H_ */
