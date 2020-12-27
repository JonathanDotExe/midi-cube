/*
 * property.h
 *
 *  Created on: Dec 27, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_PROPERTY_H_
#define MIDICUBE_GUI_ENGINE_PROPERTY_H_

#include <cstddef>

union PropertyValue {
	int ival;
	double dval;
};

class PropertyHolder {

	virtual void set(size_t prop, PropertyValue value) = 0;

	virtual PropertyValue get(size_t prop) = 0;

	virtual ~PropertyHolder() {

	}
};



#endif /* MIDICUBE_GUI_ENGINE_PROPERTY_H_ */
