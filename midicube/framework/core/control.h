/*
 * control.h
 *
 *  Created on: Nov 12, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_CONTROL_H_
#define MIDICUBE_FRAMEWORK_CORE_CONTROL_H_

#include <vector>
#include <unordered_map>

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

	unsigned int get_cc(ControlType type, size_t index, size_t bank) const {
		switch (type) {
		case SLIDER:
			if (bank < control_banks.size()) {
				const ControlBank& b = control_banks[bank];
				if (index < b.sliders.size()) {
					return b.sliders[index];
				}
			}
			return 128;
		case KNOB:
			if (bank < control_banks.size()) {
				const ControlBank& b = control_banks[bank];
				if (index < b.knobs.size()) {
					return b.knobs[index];
				}
			}
			return 128;
		case BUTTON:
			if (bank < control_banks.size()) {
				const ControlBank& b = control_banks[bank];
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

	virtual void* get_property() = 0;

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

struct ControlBind {
	ControlType type;
	size_t index = 0;
	size_t bank = 0;

	IParameter* param = nullptr;
};

class ControlHost {

public:
	virtual void notify_property_update(void* property) = 0;

	virtual const MidiControls& get_controls() = 0;

	virtual ~ControlHost() {

	}

};

class ControlView {
private:
	std::vector<ControlBind> params;
	ControlHost* plugin = nullptr;
	std::string name = "";

public:

	ControlView(std::string name) {
		this->name = name;
	}

	bool on_cc(unsigned int cc, double val) {
		bool changed = false;
		const MidiControls& controls = plugin->get_controls();
		for (ControlBind& bind : params) {
			if (cc == controls.get_cc(bind.type, bind.index, bind.bank)) {
				bind.param->change(val);
				plugin->notify_property_update(bind.param->get_property());
				changed = true;
			}
		}
		return changed;
	}

	ControlView& bind(IParameter* param, ControlType type, size_t index, size_t bank) {
		if (plugin) {
			throw "ControlView is closed for further bindings";
		}
		params.push_back({type, index, bank, param});
		return *this;
	}

	void init(ControlHost* plugin) {
		if (this->plugin) {
			throw "ControlView is already initialized";
		}
		this->plugin = plugin;
	}

	std::string get_name() const {
		return name;
	}

};


#endif /* MIDICUBE_FRAMEWORK_CORE_CONTROL_H_ */
