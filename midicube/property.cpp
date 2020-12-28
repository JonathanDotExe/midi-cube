/*
 * property.cpp
 *
 *  Created on: Dec 27, 2020
 *      Author: jojo
 */

#include "property.h"

inline void PropertyHolder::submit_change(size_t prop, PropertyValue value) {
	if (changes) {
		changes->push({this, prop, value});
	}
}

inline void PropertyHolder::submit_change(size_t prop, int value) {
	PropertyValue val;
	val.ival = value;
	submit_change(prop, val);
}

inline void PropertyHolder::submit_change(size_t prop, double value) {
	PropertyValue val;
	val.dval = value;
	submit_change(prop, val);
}

inline void PropertyHolder::submit_change(size_t prop, bool value) {
	PropertyValue val;
	val.bval = value;
	submit_change(prop, val);
}

