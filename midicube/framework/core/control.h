/*
 * control.h
 *
 *  Created on: Nov 12, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_CONTROL_H_
#define MIDICUBE_FRAMEWORK_CORE_CONTROL_H_

#include <vector>

struct ControlBank {
	std::vector<unsigned int> sliders;
	std::vector<unsigned int> knobs;
	std::vector<unsigned int> buttons;
};

enum ControlType {
	SLIDER, KNOB, BUTTON, SCENE_BUTTON, MOD_WHEEL, BREATH_CONTROLLER, VOLUME_PEDAL, EXPRESSION_PEDAL, SUSTAIN_PEDAL, SOSTENUTO_PEDAL, SOFT_PEDAL
};

struct MidiControls {
	std::vector<ControlBank> control_banks;
	std::vector<unsigned int> scene_buttons;
	unsigned int mod_wheel = 1;
	unsigned int breath_controller = 2;
	unsigned int volume_pedal = 7;
	unsigned int expresion_pedal = 11;
	unsigned int sustain_pedal = 64;
	unsigned int sostenuto_pedal = 66;
	unsigned int soft_pedal = 67;

	unsigned int get_cc(ControlType type, size_t index, size_t bank) {
		switch (type) {
		case SLIDER:
			if (bank < control_banks.size()) {
				ControlBank& b = control_banks[bank];
				if (index < b.sliders.size()) {
					return b.sliders[index];
				}
			}
			return 128;
		case KNOB:
			if (bank < control_banks.size()) {
				ControlBank& b = control_banks[bank];
				if (index < b.knobs.size()) {
					return b.knobs[index];
				}
			}
			return 128;
		case BUTTON:
			if (bank < control_banks.size()) {
				ControlBank& b = control_banks[bank];
				if (index < b.buttons.size()) {
					return b.buttons[index];
				}
			}
			return 128;
		case SCENE_BUTTON:
			if (index < scene_buttons.size()) {
				return scene_buttons[index];
			}
			return 128;
		case MOD_WHEEL:
			return mod_wheel;
		case BREATH_CONTROLLER:
			return breath_controller;
		case VOLUME_PEDAL:
			return volume_pedal;
		case EXPRESSION_PEDAL:
			return expresion_pedal;
		case SUSTAIN_PEDAL:
			return sustain_pedal;
		case SOSTENUTO_PEDAL:
			return sostenuto_pedal;
		case SOFT_PEDAL:
			return soft_pedal;
		}
		return 128;
	}
};

class IParameter {
public:

	virtual void change(double val) = 0;

	virtual ~IParameter() {

	}
};

template<typename T>
class ITemplateParameter : public IParameter {
public:

	virtual T get_min() const;

	virtual T get_max() const;

	virtual ~ITemplateParameter() {

	}

};



#endif /* MIDICUBE_FRAMEWORK_CORE_CONTROL_H_ */
