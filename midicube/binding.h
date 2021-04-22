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
	virtual void change(unsigned int value);
	virtual ~ControlBinding();
};

template <typename T>
class TemplateControlBinding : public ControlBinding {
public:
	T min;
	T max;
	T& field;

	TemplateControlBinding(T& f, T mn, T mx) : field(f), min(mn), max(mx) {

	}

	void change(unsigned int value);
};

class MidiControlHandler {
private:
	std::vector<ControlBinding*> bindings;
	bool locked = false;
public:

	void register_binding(ControlBinding* b);

	void lock();

	void on_message(MidiMessage msg);

	~MidiControlHandler() {
		for (auto b : bindings) {
			delete b;
		}
		bindings.clear();
	}
};



#endif /* MIDICUBE_BINDING_H_ */
