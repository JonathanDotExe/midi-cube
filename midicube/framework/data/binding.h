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

};

class MidiBindingHandler {
private:
	/*std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> persistent;
	std::array<std::vector<BindableValue*>, MIDI_CONTROL_COUNT> temp;*/
	std::vector<std::pair<ControlHost*, BindableValue*>> bindings;

public:

	void bind(ControlHost* source, BindableValue* value) {
		bindings.push_back({source, value});
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
		bindings.erase(std::remove_if(bindings.begin(), bindings.end(), [value](std::pair<void*, BindableValue*> v) { return v.second == value; }), bindings.end());
	}

	std::vector<std::pair<void*, void*>> on_cc(unsigned int control, double value) {
		std::vector<std::pair<void*, void*>> changes;
		for (std::pair<ControlHost*, BindableValue*> val : bindings) {
			if (val.first->get_source_controls().get_cc(val.second->type, val.second->cc, val.second->bank) == control) {
				val.second->change(value);
				changes.push_back(val); //TODO
			}
		}
		return changes;
	}

};

class LocalMidiBindingHandler {
private:
	std::vector<BindableValue*> values;
	MidiBindingHandler* handler = nullptr;
	ControlHost* source = nullptr;

public:

	void add_binding(BindableValue* value) {
		values.push_back(value);
	}

	void init(MidiBindingHandler* handler, ControlHost* source) {
		if (this->handler || this->source) {
			throw "Binding handler already initialized";
		}
		this->source = source;
		this->handler = handler;
		//Init values
		for (BindableValue* value : values) {
			handler->bind(this->source, value);
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

/*
namespace boost {
	namespace property_tree {

		template<>
		struct translator_between<ptree, BindableBooleanValue>
		{
			struct type {
				typedef ptree internal_type;
				typedef BindableBooleanValue external_type;

				boost::optional<external_type> get_value(const internal_type& tree) {
					BindableBooleanValue b = true;
					b.load(tree);
					return b;
				}

				boost::optional<internal_type> put_value(const external_type& b) {
					return b.save();
				}

			};
		};

	}
}*/


#endif /* MIDICUBE_BINDING_H_ */
