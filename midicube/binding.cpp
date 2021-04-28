/*
 * binding.cpp
 *
 *  Created on: Apr 19, 2021
 *      Author: jojo
 */

#include "binding.h"

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

std::unordered_map<std::string, unsigned int> MidiControlHandler::get_ccs() {
	std::unordered_map<std::string, unsigned int> ccs;
	for (ControlBinding* b : bindings) {
		ccs.insert(std::pair<std::string, unsigned int>(b->name, b->cc));
	}
	return ccs;
}

bool MidiControlHandler::on_message(MidiMessage msg) {
	bool updated = false;
	if (msg.type == MessageType::CONTROL_CHANGE) {
		unsigned int control = msg.control();
		unsigned int value = msg.value();
		for (ControlBinding* b : bindings) {
			if (b->cc == control) {
				b->change(value);
				updated = true;
			}
		}
	}
	return updated;
}

void MidiControlHandler::set_ccs(std::unordered_map<std::string, unsigned int> ccs) {
	for (ControlBinding* b : bindings) {
		if (ccs.find(b->name) != ccs.end()) {
			b->cc = ccs.at(b->name);
		}
		else {
			b->cc = b->default_cc;
		}
	}
}
