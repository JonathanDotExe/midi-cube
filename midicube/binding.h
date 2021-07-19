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
#include "midi.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;

class BindableValue {

public:

	//Don't edit
	unsigned int persistent_cc = 128;
	//Don't edit
	unsigned int temp_cc = 128;

	virtual void change_persistent(double val) = 0;

	virtual void change_temp(double val) = 0;

	virtual ~BindableValue() {

	}

};

//TODO boundary checks
template<typename T>
class BindableTemplateValue : public BindableValue {
private:
	T persistent_value;
	T temp_value;

	T total_min;
	T total_max;

	T temp_change = 0;

	double cc_val = 0;

	inline void recalc_temp() {
		temp_value = fmin(fmax(total_min, persistent_value + (total_max - total_min) * cc_val), total_max);
	}

public:

	BindableTemplateValue(T val, T min, T max) {
		this->persistent_value = val;
		this->total_min = min;
		this->total_max = max;
		recalc_temp();
	}

	inline T& operator=(const T& other) {
		persistent_value = other;
		recalc_temp();
		return persistent_value;
	}

	inline operator T() const {
		return persistent_value;
	}

	inline const T get_temp() const {
		return temp_value;
	}

	void change_persistent(double val) {
		persistent_value = total_min + (total_max - total_min);
		recalc_temp();
	}

	void change_temp(double val) {
		cc_val = val;
		recalc_temp();
	}

	void load(boost::property_tree::ptree& parent, std::string path, T def) {
		auto tree = parent.get_child_optional(path);
		if (tree && tree.get().get_child_optional("value")) {
			load(tree.get());
		}
		else {
			persistent_value = parent.get<T>(path, def);
			temp_change = 0;
			persistent_cc = 128;
			temp_cc = 128;
			cc_val = 0;
			recalc_temp();
		}
	}

	void load(boost::property_tree::ptree& tree) {
		persistent_value = tree.get("value", persistent_value);
		temp_change = tree.get("temp_change", temp_change);
		persistent_cc = tree.get("persistent_cc", persistent_cc);
		temp_cc = tree.get("temp_cc", temp_cc);
		cc_val = 0;
		recalc_temp();
	}

	void save(boost::property_tree::ptree& tree, std::string path) {
		tree.add_child(path, save());
	}

	boost::property_tree::ptree save() {
		boost::property_tree::ptree tree;

		tree.put("value", persistent_value);
		tree.put("temp_change", temp_change);
		tree.put("persistent_cc", persistent_cc);
		tree.put("temp_cc", temp_cc);


		return tree;
	}

};

class BindableBooleanValue : public BindableValue {
private:
	bool persistent_value;
	bool temp_value;

public:

	BindableBooleanValue(bool val = false) {
		this->persistent_value = val;
		this->temp_value = val;
	}

	inline bool operator=(const bool other) {
		persistent_value = other;
		temp_value = other;
		return persistent_value;
	}

	inline operator bool() const {
		return persistent_value;
	}

	inline const bool get_temp() const {
		return temp_value;
	}

	void change_persistent(double val) {
		persistent_value = val > 0;
		temp_value = persistent_value;
	}

	void change_temp(double val) {
		temp_value = val > 0;
	}

	void load(boost::property_tree::ptree tree) {
		persistent_value = tree.get("value", persistent_value);
	}

	boost::property_tree::ptree save() {
		boost::property_tree::ptree tree;
		tree.put("value", persistent_value);

		return tree;
	}

};

class MidiBindingHandler {
private:
	std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> persistent;
	std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> temp;

public:

	void bind(BindableValue* value, unsigned int persistent_cc, unsigned int temp_cc) {
		//Unbind
		if (value->persistent_cc < MIDI_CONTROL_COUNT) {
			std::vector<BindableValue*>& vec = persistent[value->persistent_cc];
			vec.erase(std::remove_if(vec.begin(), vec.end(), [value](BindableValue* v) { return v == value; }), vec.end());
		}
		if (value->temp_cc < MIDI_CONTROL_COUNT) {
			std::vector<BindableValue*>& vec = temp[value->temp_cc];
			vec.erase(std::remove_if(vec.begin(), vec.end(), [value](BindableValue* v) { return v == value; }), vec.end());
		}
		//Update
		value->persistent_cc = persistent_cc;
		value->temp_cc = temp_cc;
		//Bind
		if (value->persistent_cc < MIDI_CONTROL_COUNT) {
			std::vector<BindableValue*>& vec = persistent[value->persistent_cc];
			vec.push_back(value);
		}
		if (value->temp_cc < MIDI_CONTROL_COUNT) {
			std::vector<BindableValue*>& vec = temp[value->temp_cc];
			vec.push_back(value);
		}
	}

	bool on_cc(unsigned int control, double value) {
		bool updated = false;
		if (control < MIDI_CONTROL_COUNT) {
			for (BindableValue* val : persistent[control]) {
				val->change_persistent(value);
				updated = true;
			}
			for (BindableValue* val : temp[control]) {
				val->change_temp(value);
				updated = true;
			}
		}
		return updated;
	}

};

class ControlBinding {
public:
	unsigned int cc = 0;
	unsigned int default_cc = 0;
	std::string name;
	ControlBinding(std::string n) : name(n) {

	}
	virtual void change(unsigned int value) = 0;
	virtual void* var() = 0;
	virtual ~ControlBinding() {

	}
};

template <typename T>
class TemplateControlBinding : public ControlBinding {
public:
	T& field;
	T min;
	T max;

	TemplateControlBinding(std::string name, T& f, T mn, T mx, unsigned int cc = 128) : ControlBinding(name), field(f), min(mn), max(mx) {
		this->cc = cc;
		this->default_cc = cc;
	}

	void change(unsigned int value);

	void* var() {
		return &field;
	}
};

template <typename T>
void TemplateControlBinding<T>::change(unsigned int value) {
	field = value/127.0 * (max - min) + min;
}


class MidiControlHandler {
private:
	std::vector<ControlBinding*> bindings;
	bool locked = false;
public:

	void set_ccs(std::unordered_map<std::string, unsigned int> ccs);

	std::unordered_map<std::string, unsigned int> get_ccs();

	void register_binding(ControlBinding* b);

	ControlBinding* get_binding(void* field) {
		for (auto b : bindings) {
			if (b->var() == field) {
				return b;
			}
		}
		return nullptr;
	}

	void lock();

	bool on_message(MidiMessage msg);

	~MidiControlHandler() {
		for (auto b : bindings) {
			delete b;
		}
		bindings.clear();
	}
};



#endif /* MIDICUBE_BINDING_H_ */
