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
#include <boost/lockfree/queue.hpp>

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
};

/**
 * Functions are intended to be called from the UI thread
 */
class PropertyHolder {

public:

	boost::lockfree::queue<PropertyChange>* changes = nullptr;

	void request_update() {
		update_request = true;
	}

	virtual void set(size_t prop, PropertyValue value) = 0;

	virtual PropertyValue get(size_t prop) = 0;

	virtual ~PropertyHolder() {

	}

protected:
	std::atomic<bool> update_request;

	inline void submit_change(size_t prop, PropertyValue value);

	inline void submit_change(size_t prop, int value);

	inline void submit_change(size_t prop, double value);

	inline void submit_change(size_t prop, bool value);
};

void PropertyHolder::submit_change(size_t prop, PropertyValue value) {
	if (changes) {
		changes->push({this, prop, value});
	}
}

void PropertyHolder::submit_change(size_t prop, int value) {
	PropertyValue val;
	val.ival = value;
	submit_change(prop, val);
}

void PropertyHolder::submit_change(size_t prop, double value) {
	PropertyValue val;
	val.dval = value;
	submit_change(prop, val);
}

void PropertyHolder::submit_change(size_t prop, bool value) {
	PropertyValue val;
	val.bval = value;
	submit_change(prop, val);
}


#endif /* MIDICUBE_GUI_ENGINE_PROPERTY_H_ */
