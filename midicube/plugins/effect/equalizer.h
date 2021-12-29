/*
 * Equalizer.h
 *
 *  Created on: May 27, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_EQUALIZER_H_
#define MIDICUBE_EFFECT_EQUALIZER_H_

#include <string>
#include "../../framework/core/audio.h"
#include "../../framework/dsp/oscilator.h"
#include "../../framework/dsp/equalizer.h"
#include "../../framework/core/plugins/effect.h"

#define EQUALIZER_IDENTIFIER "midicube_4_band_eq"

struct EqualizerPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> low_freq{100, 20, 400};
	BindableTemplateValue<double> low_gain{0, -1, 5};
	BindableTemplateValue<double> low_mid_freq{400, 100, 1000};
	BindableTemplateValue<double> low_mid_gain{0, -1, 5};
	BindableTemplateValue<double> mid_freq{1000, 200, 8000};
	BindableTemplateValue<double> mid_gain{0, -1, 5};
	BindableTemplateValue<double> high_freq{4000, 1000, 20000};
	BindableTemplateValue<double> high_gain{0, -1, 5};
};


class EqualizerProgram : public PluginProgram {
public:
	EqualizerPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~EqualizerProgram() {

	}
};

#define EQ_4_BANDS 4

class EqualizerEffect : public Effect {
private:
	NBandEqualizer<EQ_4_BANDS> leq;
	NBandEqualizer<EQ_4_BANDS> req;

public:
	EqualizerPreset preset;

	void init();
	EqualizerEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	Menu* create_menu();
	~EqualizerEffect();
};

class EqualizerPlugin : public EffectPlugin<EqualizerEffect, EqualizerProgram> {
public:
	EqualizerPlugin() : EffectPlugin({
		"Equalizer",
		EQUALIZER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_EQUALIZER_H_ */
