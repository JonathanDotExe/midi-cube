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
