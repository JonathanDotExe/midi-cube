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
#define AMPLIFIER_SIMULATION_EQ_BANDS 3

struct AmplifierSimulationPreset {
	BindableBooleanValue on{true};
	BindableTemplateValue<double> post_gain{0, 0, 1};
	BindableTemplateValue<double> drive{0, 0, 1};
	BindableTemplateValue<double> tone{0.8, 0, 1};
	BindableTemplateValue<double> low_freq{100, 20, 400};
	BindableTemplateValue<double> low_gain{0, -1, 5};
	BindableTemplateValue<double> mid_freq{1000, 200, 8000};
	BindableTemplateValue<double> mid_gain{0, -1, 5};
	BindableTemplateValue<double> high_freq{4000, 1000, 20000};
	BindableTemplateValue<double> high_gain{0, -1, 5};
	DistortionType type = TANH_DISTORTION;
	BindableTemplateValue<double> lowshelf_cutoff{80, 15, 160};
	BindableTemplateValue<double> lowshelf_boost{0, 0, 1};
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
	AmplifierSimulation<AMPLIFIER_SIMULATION_EQ_BANDS> lamp;
	AmplifierSimulation<AMPLIFIER_SIMULATION_EQ_BANDS> ramp;
public:
	AmplifierSimulationPreset preset;

	void init();
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	AmplifierSimulationEffect(PluginHost& h, Plugin& plugin);
	void process(const SampleInfo& info);
	Menu* create_menu();

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
