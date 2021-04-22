/*
 * binding.cpp
 *
 *  Created on: Apr 19, 2021
 *      Author: jojo
 */

#include "binding.h"

template <typename T>
void TemplateControlBinding<T>::change(unsigned int value) {
	field = value/127.0 * (max - min) + min;
}


void MidiControlHandler::register_binding(ControlBinding *b) {
	if (!locked) {
		bindings.push_back(b);
	}
	else {
		delete b;
		throw "Control handler already locked!";
	}
}

void MidiControlHandler::lock() {
	locked = true;
}

void MidiControlHandler::on_message(MidiMessage msg) {
	if (msg.type == MessageType::CONTROL_CHANGE) {
		unsigned int control = msg.control();
		unsigned int value = msg.value();
		for (ControlBinding* b : bindings) {
			if (b->cc == control) {
				b->change(value);
			}
		}
	}
}
