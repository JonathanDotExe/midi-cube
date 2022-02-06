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
#include "../dsp/synthesis.h"

#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

enum ControlType {
	CC, SLIDER, KNOB, BUTTON, SCENE_BUTTON, MOD_WHEELS, BREATH_CONTROLLER, FOOT_SWITCHES, EXPRESSION_PEDALS, SUSTAIN_PEDAL, SOSTENUTO_PEDAL, SOFT_PEDAL
};

struct MidiControls {
	std::vector<unsigned int> sliders{67, 68, 69, 70, 87, 88, 89, 90, 92, 73, 75, 79, 72, 80, 81, 82, 83, 84};
	std::vector<unsigned int> knobs{35, 36, 37, 38, 39, 40, 41, 42, 43, 74, 71, 76, 77, 93, 18, 19, 16, 17};
	std::vector<unsigned int> buttons{22, 23, 24, 25, 26, 27, 28, 29, 30};
	std::vector<unsigned int> scene_buttons = {52, 53, 54, 55, 56, 57, 58, 59, 60, 61};
	std::vector<unsigned int> mod_wheels{1};
	std::vector<unsigned int> expression_pedals{11, 7};
	std::vector<unsigned int> foot_switches{};
	unsigned int breath_controller = 2;
	unsigned int sustain_pedal = 64;
	unsigned int sostenuto_pedal = 66;
	unsigned int soft_pedal = 67;

	unsigned int get_cc(ControlType type, size_t index) const {
		switch (type) {
		case CC:
			return index;
		case SLIDER:
			if (index < sliders.size()) {
				return sliders[index];
			}
			return 128;
		case KNOB:
			if (index < knobs.size()) {
				return knobs[index];
			}
			return 128;
		case BUTTON:
			if (index < buttons.size()) {
				return buttons[index];
			}
			return 128;
		case SCENE_BUTTON:
			if (index < scene_buttons.size()) {
				return scene_buttons[index];
			}
			return 128;
		case MOD_WHEELS:
			if (index < mod_wheels.size()) {
				return mod_wheels[index];
			}
			return 128;
		case BREATH_CONTROLLER:
			return breath_controller;
		case FOOT_SWITCHES:
			if (index < foot_switches.size()) {
				return foot_switches[index];
			}
			return 128;
		case EXPRESSION_PEDALS:
			if (index < expression_pedals.size()) {
				return expression_pedals[index];
			}
			return 128;
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

class ControlHost;

class IBindable {
private:
	ControlHost* host = nullptr;

protected:
	inline void notify_poperty_change() {
		host->notify_property_update(this);
	}

public:
	const std::string name;


	IBindable(std::string name) {
		this->name = name;
	}

	void init (ControlHost* host) {
		if (this->host == nullptr) {
			this->host = host;
		}
	}

	inline ControlHost* get_host() {
		return host;
	}

	virtual void change(double val) = 0;

	virtual double get_scaled() = 0;

	virtual void* get_property() = 0;

	virtual void save() = 0;

	virtual ~IBindable();

};

struct MidiBinding {
	ControlType type;
	size_t index = 0;
	double amount = 0;
	bool persistent = true;
};


struct MidiBindingCollection {
	double start_value = 0;
	std::vector<std::pair<MidiBinding, double>> bindings;
};


class ControlHost {

private:
	std::unordered_map<IBindable*, MidiBindingCollection> bindings;
	std::vector<IBindable*> parameters;

protected:

	void add_paramter(IBindable* p) {
		p->init(this);
		parameters.push_back(p);
	}

public:
	virtual void notify_property_update(void* property) const = 0;

	virtual const MidiControls& get_controls() const = 0;

	//Parameter functions
	std::vector<IBindable*> get_parameters() {
		return parameters;
	}

	//Binding functions
	void bind(IBindable* param, MidiBinding binding) {
		if (param->get_host() != this) {
			throw "Can bind parameter, that is not attached to this host!";
		}
		if (!bindings.count(param)) {
			bindings[param] = {};
		}
		bindings[param].bindings.push_back({binding, {0}});
	}

	void unbind(IBindable* value) {
		bindings.erase(value);
	}

	void unbind(IBindable* value, int index) {
		bindings[value].bindings.erase(bindings[value].bindings.begin() + index);
	}

	std::vector<MidiBinding> get_bindings(IBindable* param) {
		if (!bindings.count(param)) {
			return {};
		}
		std::vector<MidiBinding> b;
		for (auto& p : bindings[param].bindings) {
			b.push_back(p.first);
		}
		return b;
	}

	std::vector<std::pair<MidiBinding, double>> get_binding_values(IBindable* param) {
		if (!bindings.count(param)) {
			return {};
		}
		return bindings[param].bindings;
	}

	//Has to be called from subclass
	void on_cc(unsigned int control, double value) {
		for (auto& val : bindings) {
			const MidiControls& controls = get_controls();
			bool changed = false;
			//Apply controls
			for (std::pair<MidiBinding, double>& b : val.second.bindings) {
				if (controls.get_cc(b.first.type, b.first.index) == control) {
					b.second = value;
					changed = true;
				}
			}
			//Changed
			if (changed) {
				double v = val.second.start_value;
				for (std::pair<MidiBinding, double>& b : val.second.bindings) {
					v += b.second * b.first.amount;
				}
				val.first->change(std::min(std::max(v, 0.0), 1.0));
			}
		}
	}

	//Has to be called from subclass
	void process(double time_step) {
		//TODO parameter smoothing
	}

	virtual ~ControlHost() {

	}

};

template<typename T>
class IParameter : public IBindable {
public:

	IParameter(std::string name) : IBindable(name) { }

	virtual T get() = 0;

	virtual void set(T val) = 0;

	virtual T scale(double val) = 0;

	virtual void change(double val) {
		set(scale(val));
		notify_poperty_change();
	}

	//Save load
	virtual void put_value(pt::ptree& tree, T val) {
		tree.put_value(val);
	}

	virtual pt::ptree save() {
		pt::ptree t;
		std::vector<std::pair<MidiBinding, double>> bindings = get_host()->get_bindings(this);
		if (bindings.size()) {
			double value = get_scaled();
			for (auto& b : bindings) {
				//Remove temporary changes
				if (!b.first.persistent) {
					value -= b.second * b.first.amount;
				}
			}
			pt::ptree val;
			put_value(val, scale(value));
			t.put_child("value", val);
			for (auto& value : bindings) {
				pt::ptree binding;
				binding.put("type", value.first.type);
				binding.put("index", value.first.index);
				binding.put("amount", value.first.amount);
				binding.put("persistent", value.first.persistent);
				t.add_child("bindings.binding", binding);
			}
		}
		else {
			put_value(t, get());
		}
		return t;
	}

	virtual ~IParameter() {

	}
};

template<typename T>
class ITemplateParameter : public IParameter<T> {
public:

	ITemplateParameter(std::string name) : IParameter<T>(name) { }

	virtual T get_min() const = 0;

	virtual T get_max() const = 0;

	virtual ~ITemplateParameter() {

	}

};

class IConverterFunction {
public:
	virtual double apply(double x) const = 0;
	virtual double inverse(double x) const = 0;
	virtual ~IConverterFunction() {

	}
};

template<typename T>
class TemplateParameter : public ITemplateParameter<T> {
private:
	T& variable;
	T total_min;
	T total_max;
	const IConverterFunction& converter;
	const IConverterFunction& curve;

public:

	void* get_property() {
		return this;
	}

	TemplateParameter(T& v, T min, T max, const IConverterFunction& c, const IConverterFunction& cu, std::string name) : ITemplateParameter<T>(name), variable(v), total_min(min), total_max(max), converter(c), curve(c) {

	}

	T scale(double val) {
		return total_min + (total_max - total_min) * curve.apply(val);
	}

	T get_min() const {
		return total_min;
	}

	T get_max() const {
		return total_max;
	}

	T get() {
		return converter.inverse(variable);
	}

	void set(T val) {
		variable = converter.apply(val);
	}

};

template<typename T>
class TemplateEnumParameter : public ITemplateParameter<T> {
private:
	T& variable;
	std::vector<std::pair<T, std::string>> values;

public:

	void* get_property() {
		return this;
	}

	TemplateEnumParameter(T& v, std::vector<std::pair<T, std::string>>, ControlHost* host, std::string name) : ITemplateParameter<T>(host, name), variable(v){

	}

	T scale(double val) {
		return values[(values.size() - 1) * val];
	}

	T get_min() const {
		return total_min;
	}

	T get_max() const {
		return total_max;
	}

	T get() {
		return variable;
	}

	void set(T val) {
		variable = val;
	}

	const std::vector<std::pair<T, std::string>>& get_values() {
		return values;
	}

};

class BooleanParameter : public IParameter<bool> {
private:
	bool& value;

public:

	void* get_property() {
		return this;
	}

	BooleanParameter(bool& val) : value(val){

	}

	bool scale(double val) {
		return value = val > 0.5;
	}

	bool get() {
		return value;
	}

	void set(bool val) {
		value = val;
	}

};


struct ControlViewBind {
	ControlType type;
	size_t index = 0;
	IBindable* param = nullptr;
};


class ControlView {
private:
	std::vector<ControlViewBind> params;
	ControlHost* plugin = nullptr;
	std::string name = "";

public:

	ControlView(std::string name) {
		this->name = name;
	}

	bool on_cc(unsigned int cc, double val) {
		bool changed = false;
		const MidiControls& controls = plugin->get_controls();
		for (ControlViewBind& bind : params) {
			if (cc == controls.get_cc(bind.type, bind.index)) {
				bind.param->change(val);
				plugin->notify_property_update(bind.param->get_property());
				changed = true;
			}
		}
		return changed;
	}

	ControlView& bind(IBindable* param, ControlType type, size_t index, size_t bank) {
		if (plugin) {
			throw "ControlView is closed for further bindings";
		}
		params.push_back({type, index, param});
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
