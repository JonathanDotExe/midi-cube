/*
 * binding.h
 *
 *  Created on: 19 Apr 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_BINDING_H_
#define MIDICUBE_BINDING_H_

#include <vector>
#include <unordered_map>
#include "midi.h"

template<typename T>
class BindableValue {
private:
	T persistent_value;
	T temp_value;

	T total_min;
	T total_max;

	T temp_minchange = 1;

	double cc_val = 0;

	inline void recalc_temp() {
		temp_value = fmin(fmax(total_min, persistent_value + (total_max - total_min) * cc_val), total_max);
	}

public:

	unsigned int persistent_cc = 128;
	unsigned int temp_cc = 128;

	BindableValue(T val = 0, T min = 0, T max = 0) {
		this->persistent_value = val;
		this->total_min = min;
		this->total_max = max;
	}

	inline T& operator=(const T& other) {
		persistent_value = other;
		temp_value = other;
		return persistent_value;
	}

	inline operator T() const {
		return temp_value;
	}

	inline const T get_persistent() const {
		return persistent_value;
	}

	bool control_change(unsigned int control, double val) {
		bool changed = false;
		if (control == persistent_cc) {
			persistent_value = total_min + (total_max - total_min) * val;
			changed = true;
		}
		if (control == temp_cc) {
			cc_val = val;
			changed = true;
		}
		if (changed) {
			recalc_temp();
		}
		return changed;
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
