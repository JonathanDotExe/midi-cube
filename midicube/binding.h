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

	bool persistent = true;
	unsigned int cc = 128;

	virtual void change(double val) = 0;

	virtual ~BindableValue() {

	}

};

//TODO boundary checks
template<typename T>
class BindableTemplateValue : public BindableValue {
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
			persistent = true;
		}
	}

	void load(boost::property_tree::ptree& tree) {
		value = tree.get("value", default_value);
		default_value = value;
		binding_min = tree.get("binding_min", binding_min);
		binding_max = tree.get("binding_max", binding_max);
		cc = tree.get("cc", cc);
		persistent = tree.get("persistent", persistent);
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
		tree.put("persistent", persistent);


		return tree;
	}

};

class BindableBooleanValue : public BindableValue {
private:
	bool value;
	bool default_value;

public:

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
		value = val > 0;
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
			persistent = true;
		}
	}

	void load(boost::property_tree::ptree tree) {
		value = tree.get("value", default_value);
		default_value = value;
		cc = tree.get("cc", cc);
		persistent = tree.get("persistent", persistent);
	}

	void save(boost::property_tree::ptree& tree, std::string path) {
		tree.add_child(path, save());
	}

	boost::property_tree::ptree save() {
		boost::property_tree::ptree tree;
		tree.put("value", default_value);
		tree.put("cc", cc);
		tree.put("persistent", persistent);

		return tree;
	}

};

class MidiBindingHandler {
private:
	/*std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> persistent;
	std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> temp;*/
	std::vector<BindableValue*> bindings;

public:

	void bind(BindableValue* value) {
		bindings.push_back(value);
		/*//Unbind
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
		}*/
	}

	void unbind(BindableValue* value) {
		//Unbind
		/*if (value->persistent_cc < MIDI_CONTROL_COUNT) {
			std::vector<BindableValue*>& vec = persistent[value->persistent_cc];
			vec.erase(std::remove_if(vec.begin(), vec.end(), [value](BindableValue* v) { return v == value; }), vec.end());
		}
		if (value->temp_cc < MIDI_CONTROL_COUNT) {
			std::vector<BindableValue*>& vec = temp[value->temp_cc];
			vec.erase(std::remove_if(vec.begin(), vec.end(), [value](BindableValue* v) { return v == value; }), vec.end());
		}*/
		bindings.erase(std::remove_if(bindings.begin(), bindings.end(), [value](BindableValue* v) { return v == value; }), bindings.end());
	}

	bool on_cc(unsigned int control, double value) {
		bool updated = false;
		for (BindableValue* val : bindings) {
			if (val->cc == control) {
				val->change(value);
			}
			updated = true;
		}
		return updated;
	}

};

class LocalMidiBindingHandler {
private:
	std::vector<BindableValue*> values;
	MidiBindingHandler* handler = nullptr;

public:

	void add_binding(BindableValue* value) {
		values.push_back(value);
	}

	void init(MidiBindingHandler* handler) {
		this->handler = handler;
		//Init values
		for (BindableValue* value : values) {
			handler->bind(value);
		}
	}

	void unbind() {
		if (handler) {
			for (BindableValue* value : values) {
				handler->unbind(value);
			}
		}
	}

	~LocalMidiBindingHandler() {
		unbind();
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
