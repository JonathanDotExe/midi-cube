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
#include <iostream>

struct ControlBank {
	std::vector<unsigned int> sliders;
	std::vector<unsigned int> knobs;
	std::vector<unsigned int> buttons;
};

enum ControlType {
	CC, SLIDER, KNOB, BUTTON, SCENE_BUTTON, MOD_WHEEL, BREATH_CONTROLLER, VOLUME_PEDAL, EXPRESSION_PEDAL, SUSTAIN_PEDAL, SOSTENUTO_PEDAL, SOFT_PEDAL
};

struct MidiControls {
	std::vector<ControlBank> control_banks = {{
			{67, 68, 69, 70, 87, 88, 89, 90, 92},
			{35, 36, 37, 38, 39, 40, 41, 42, 43},
			{22, 23, 24, 25, 26, 27, 28, 29, 30}
	},
	{
			{73, 75, 79, 72, 80, 81, 82, 83, 84},
			{74, 71, 76, 77, 93, 18, 19, 16, 17},
			{128, 128, 128, 128, 128, 128, 128, 128, 128}
	}};
	std::vector<unsigned int> scene_buttons = {52, 53, 54, 55, 56, 57, 58, 59, 60, 61};
	unsigned int mod_wheel = 1;
	unsigned int breath_controller = 2;
	unsigned int volume_pedal = 7;
	unsigned int expresion_pedal = 11;
	unsigned int sustain_pedal = 64;
	unsigned int sostenuto_pedal = 66;
	unsigned int soft_pedal = 67;

	unsigned int get_cc(ControlType type, size_t index, size_t bank) const {
		switch (type) {
		case CC:
			return index;
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

	virtual T get_min() const = 0;

	virtual T get_max() const = 0;

	virtual ~ITemplateParameter() {

	}

};

template<typename T>
class TemplateParameter : public ITemplateParameter<T> {
private:
	T value;

public:

	//DONT CHANGE
	T total_min;
	//DONT CHANGE
	T total_max;

	void* get_property() {
		return this;
	}

	TemplateParameter(T val, T min, T max) : total_min(min), total_max(max) {
		this->value = val;
	}

	TemplateParameter(const TemplateParameter<T>& other) {
		this->value = other.value;
		this->total_min = other.total_min;
		this->total_max = other.total_max;
	}

	inline TemplateParameter<T>& operator=(const TemplateParameter<T>& other) {
		if (&other != this) {
			this->value = other.value;
			this->total_min = other.total_min;
			this->total_max = other.total_max;
		}
		return *this;
	}

	inline T& operator=(const T& other) {
		this->value = other;
		return value;
	}

	inline operator T() const {
		return value;
	}

	void change(double val) {
		value = total_min + (total_max - total_min) * val;
	}

	T get_min() const {
		return total_min;
	}

	T get_max() const {
		return total_max;
	}

};

template<typename T>
class TemplateEnumParameter : public ITemplateParameter<T> {
private:
	T value;

public:

	//DONT CHANGE
	T total_min;
	//DONT CHANGE
	T total_max;

	void* get_property() {
		return this;
	}

	TemplateEnumParameter(T val, T min, T max) : total_min(min), total_max(max) {
		this->value = val;
	}

	TemplateEnumParameter(const TemplateParameter<T>& other) {
		this->value = other.value;
		this->total_min = other.total_min;
		this->total_max = other.total_max;
	}

	inline TemplateParameter<T>& operator=(const TemplateParameter<T>& other) {
		if (&other != this) {
			this->value = other.value;
			this->total_min = other.total_min;
			this->total_max = other.total_max;
		}
		return *this;
	}

	inline T& operator=(const T& other) {
		this->value = other;
		return value;
	}

	inline int operator=(const int& other) {
		this->value = static_cast<T>(other);
		return other;
	}

	inline operator T() const {
		return value;
	}

	inline operator int() const {
		return static_cast<int>(value);
	}

	void change(double val) {
		value = static_cast<T>(static_cast<int>(total_min) + (static_cast<int>(total_max) - static_cast<int>(total_min)) * val);
	}

	T get_min() const {
		return total_min;
	}

	T get_max() const {
		return total_max;
	}

};

class BooleanParameter : public IParameter {
private:
	bool value;

public:

	void* get_property() {
		return this;
	}

	BooleanParameter(bool val = false) {
		this->value = val;
	}

	BooleanParameter(const BooleanParameter& other) {
		this->value = other.value;
	}

	inline bool operator=(const bool other) {
		value = other;
		return value;
	}

	inline operator bool() const {
		return value;
	}
	void change(double val) {
		value = val > 0;
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

class ControlViewHost {
public:
	virtual void change_control_view(ControlView* view) = 0;
	virtual ~ControlViewHost() {

	}
};


#endif /* MIDICUBE_FRAMEWORK_CORE_CONTROL_H_ */
