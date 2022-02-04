/*
 * binding.h
 *
 *  Created on: 19 Apr 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_BINDING_H_
#define MIDICUBE_BINDING_H_

#include <vector>
#include <array>
#include <unordered_map>
#include <algorithm>
#include "../core/midi.h"
#include "../core/control.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;

/*
class BindableValue {

public:

	bool persistent = true;
	unsigned int cc = 128;
	unsigned int bank = 0;
	ControlType type = ControlType::CC;

	virtual void change(double val) = 0;

	virtual ~BindableValue() {

	}

};

//TODO boundary checks
template<typename T>
class BindableTemplateValue : public BindableValue, public ITemplateParameter<T> {
private:
	T value;
	T default_value;

public:

	//DONT CHANGE
	T total_min;
	//DONT CHANGE
	T total_max;

	T binding_min;
	T binding_max;

	void* get_property() {
		return this;
	}

	BindableTemplateValue(T val, T min, T max) : total_min(min), total_max(max) {
		this->value = val;
		this->default_value = val;
		this->binding_min = min;
		this->binding_max = max;
	}

	BindableTemplateValue(const BindableTemplateValue<T>& other) {
		this->value = other.default_value;
		this->default_value = other.default_value;
		this->binding_min = other.binding_min;
		this->binding_max = other.binding_max;
		this->total_min = other.total_min;
		this->total_max = other.total_max;
		this->cc = other.cc;
		this->persistent = other.persistent;
		this->type = other.type;
	}

	inline BindableTemplateValue<T>& operator=(const BindableTemplateValue<T>& other) {
		if (&other != this) {
			this->value = other.default_value;
			this->default_value = other.default_value;
			this->binding_min = other.binding_min;
			this->binding_max = other.binding_max;
			this->total_min = other.total_min;
			this->total_max = other.total_max;
			this->cc = other.cc;
			this->persistent = other.persistent;
			this->type = other.type;
		}
		return *this;
	}

	inline T& operator=(const T& other) {
		this->value = other;
		this->default_value = other;
		return value;
	}

	inline operator T() const {
		return value;
	}

	inline const T get_default() const {
		return default_value;
	}

	void change(double val) {
		value = binding_min + (binding_max - binding_min) * val;
		if (persistent) {
			default_value = value;
		}
	}

	void load(boost::property_tree::ptree& parent, std::string path, T def) {
		auto tree = parent.get_child_optional(path);
		if (tree && tree.get().get_child_optional("value")) {
			load(tree.get());
		}
		else {
			value = parent.get<T>(path, def);
			default_value = value;
			binding_min = total_min;
			binding_max = total_max;
			cc = 128;
			bank = 0;
			persistent = true;
			type = ControlType::CC;
		}
	}

	void load(boost::property_tree::ptree& tree) {
		value = tree.get("value", default_value);
		default_value = value;
		binding_min = tree.get("binding_min", binding_min);
		binding_max = tree.get("binding_max", binding_max);
		cc = tree.get("cc", cc);
		bank = tree.get("bank", bank);
		persistent = tree.get("persistent", persistent);
		type = static_cast<ControlType>(tree.get<int>("type", type));
	}

	void save(boost::property_tree::ptree& tree, std::string path) {
		tree.add_child(path, save());
	}

	boost::property_tree::ptree save() {
		boost::property_tree::ptree tree;

		tree.put("value", default_value);
		tree.put("binding_min", binding_min);
		tree.put("binding_max", binding_max);
		tree.put("cc", cc);
		tree.put("bank", bank);
		tree.put("persistent", persistent);
		tree.put("type", static_cast<int>(type));

		return tree;
	}

	T get_min() const {
		return total_min;
	}

	T get_max() const {
		return total_max;
	}

};

class BindableBooleanValue : public BindableValue, public IParameter {
private:
	bool value;
	bool default_value;

public:

	void* get_property() {
		return this;
	}

	BindableBooleanValue(bool val = false) {
		this->value = val;
		this->default_value = val;
	}

	BindableBooleanValue(const BindableBooleanValue& other) {
		this->value = other.default_value;
		this->default_value = other.default_value;
		this->cc = other.cc;
		this->persistent = other.persistent;
	}

	inline bool operator=(const bool other) {
		value = other;
		default_value = other;
		return value;
	}

	inline operator bool() const {
		return value;
	}

	inline const bool get_default() const {
		return default_value;
	}

	void change(double val) {
		value = val >= 0.5;
		if (persistent) {
			default_value = value;
		}
	}

	void load(boost::property_tree::ptree& parent, std::string path, bool def) {
		auto tree = parent.get_child_optional(path);
		if (tree && tree.get().get_child_optional("value")) {
			load(tree.get());
		}
		else {
			value = parent.get<bool>(path, def);
			default_value = value;
			cc = 128;
			bank = 0;
			persistent = true;
			type = ControlType::CC;
		}
	}

	void load(boost::property_tree::ptree tree) {
		value = tree.get("value", default_value);
		default_value = value;
		cc = tree.get("cc", cc);
		bank = tree.get("bank", bank);
		persistent = tree.get("persistent", persistent);
		type = static_cast<ControlType>(tree.get<int>("type", type));

	}

	void save(boost::property_tree::ptree& tree, std::string path) {
		tree.add_child(path, save());
	}

	boost::property_tree::ptree save() const {
		boost::property_tree::ptree tree;
		tree.put("value", default_value);
		tree.put("cc", cc);
		tree.put("bank", bank);
		tree.put("persistent", persistent);
		tree.put("type", static_cast<int>(type));

		return tree;
	}

};*/

struct MidiBinding {
	ControlType type;
	size_t index = 0;
	double amount = 0;
	bool persistent = true;
	double cc_value = 0;
};

struct MidiBindingCollection {
	double start_value = 0;
	std::vector<MidiBinding> bindings;
};

class MidiBindingHandler {
private:
	/*std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> persistent;
	std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> temp;*/
	std::unordered_map<IBindable*, MidiBindingCollection> bindings;

public:

	void bind(IBindable* param, MidiBinding binding) {
		binding.cc_value = 0; // TODO maybe create extra data stucture
		if (!bindings.count(param)) {
			bindings[param] = {};
		}
		bindings[param].bindings.push_back(binding);
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
		return param;
	}

	void on_cc(unsigned int control, double value) {
		for (auto& val : bindings) {
			const MidiControls& controls = val.first->host->get_controls();
			bool changed = false;
			//Apply controls
			for (MidiBinding& b : val.second.bindings) {
				if (controls.get_cc(b.type, b.index) == control) {
					b.cc_value = value;
				}
			}
			//Changed
			if (changed) {
				double v = val.second.start_value;
				for (MidiBinding& b : val.second.bindings) {
					v += b.cc_value * b.amount;
				}
				val.first->change(std::min(std::max(v, 0.0), 1.0));
			}
		}
	}

};

#endif /* MIDICUBE_BINDING_H_ */
