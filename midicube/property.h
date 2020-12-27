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
	std::function<void(PropertyChange)> listener;

	virtual void set(size_t prop, PropertyValue value) = 0;

	virtual PropertyValue get(size_t prop) = 0;

	virtual ~PropertyHolder() {

	}
};



#endif /* MIDICUBE_GUI_ENGINE_PROPERTY_H_ */
