/*
 * binding.h
 *
 *  Created on: 19 Apr 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_BINDING_H_
#define MIDICUBE_BINDING_H_

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
public:

};



#endif /* MIDICUBE_BINDING_H_ */
