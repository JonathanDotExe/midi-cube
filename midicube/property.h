/*
 * property.h
 *
 *  Created on: Dec 27, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_PROPERTY_H_
#define MIDICUBE_GUI_ENGINE_PROPERTY_H_

#include <cstddef>
#include <functional>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>

template<typename T>
struct PropertyType {};

union PropertyValue {
	int ival;
	double dval;
	bool bval;

	int get(PropertyType<int> t) {
		return ival;
	}

	double get(PropertyType<double> t) {
		return dval;
	}

	bool get(PropertyType<bool> t) {
		return bval;
	}

};

class PropertyHolder;

struct PropertyChange {
	PropertyHolder* holder;
	size_t property;
	PropertyValue value;
	size_t sub_property = 0;
};

/**
 * Functions are intended to be called from the UI thread
 */
class PropertyHolder {

public:

	boost::lockfree::spsc_queue<PropertyChange>* changes = nullptr;

	virtual void set(size_t prop, PropertyValue value, size_t sub_prop = 0) = 0;

	virtual PropertyValue get(size_t prop, size_t sub_prop = 0) = 0;

	virtual void update_properties() = 0;

	virtual ~PropertyHolder() {

	}

protected:

	inline void submit_change(size_t prop, PropertyValue value, size_t sub_prop = 0);

	inline void submit_change(size_t prop, int value, size_t sub_prop = 0);

	inline void submit_change(size_t prop, double value, size_t sub_prop = 0);

	inline void submit_change(size_t prop, bool value, size_t sub_prop = 0);
};

void PropertyHolder::submit_change(size_t prop, PropertyValue value, size_t sub_prop) {
	if (changes) {
		if (!changes->push({this, prop, value, sub_prop})) {
			std::cerr << "Lost change message " << prop << "/" << sub_prop << std::endl;
		}
	}
}

void PropertyHolder::submit_change(size_t prop, int value, size_t sub_prop) {
	PropertyValue val;
	val.ival = value;
	submit_change(prop, val, sub_prop);
}

void PropertyHolder::submit_change(size_t prop, double value, size_t sub_prop) {
	PropertyValue val;
	val.dval = value;
	submit_change(prop, val, sub_prop);
}

void PropertyHolder::submit_change(size_t prop, bool value, size_t sub_prop) {
	PropertyValue val;
	val.bval = value;
	submit_change(prop, val, sub_prop);
}


#endif /* MIDICUBE_GUI_ENGINE_PROPERTY_H_ */
