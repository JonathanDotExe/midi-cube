/*
 * amplifier_simulation.h
 *
 *  Created on: Jan 7, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_
#define MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_

#include "../../framework/core/plugins/effect.h"
#include "../../framework/core/audio.h"
#include "../../framework/dsp/filter.h"
#include "../../framework/dsp/distortion.h"
#include "../../framework/dsp/synthesis.h"

#define AMP_OVERSAMPLING 2
#define AMPLIFIER_SIMULATION_IDENTIFIER "midicube_overdrive_amp"

struct AmplifierSimulationPreset {
	BindableBooleanValue on{true};
	BindableTemplateValue<double> post_gain{0, 0, 1};
	BindableTemplateValue<double> drive{0, 0, 1};
	BindableTemplateValue<double> tone{0.6, 0, 1};
	DistortionType type = ARCTAN_DISTORTION;
};

class AmplifierSimulationProgram : public PluginProgram {
public:
	AmplifierSimulationPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();


	virtual ~AmplifierSimulationProgram() {

	}
};

class AmplifierSimulationEffect : public Effect {
private:
	AmplifierSimulation lamp;
	AmplifierSimulation ramp;
public:
	AmplifierSimulationPreset preset;

	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	AmplifierSimulationEffect(PluginHost& h, Plugin& plugin);
	void process(const SampleInfo& info);
	ViewController* create_view();

	~AmplifierSimulationEffect();
};

class AmplifierSimulationPlugin : public EffectPlugin<AmplifierSimulationEffect, AmplifierSimulationProgram> {
public:
	AmplifierSimulationPlugin() : EffectPlugin({
		"Amplifier",
		AMPLIFIER_SIMULATION_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_ */
