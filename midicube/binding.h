/*
 * binding.h
 *
 *  Created on: 19 Apr 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_BINDING_H_
#define MIDICUBE_BINDING_H_

#include <vector>
#include "midi.h"

class ControlBinding {
public:
	unsigned int cc = 0;
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
