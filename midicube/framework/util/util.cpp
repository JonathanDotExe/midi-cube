/*
 * util.cpp
 *
 *  Created on: 29 Jun 2020
 *      Author: jojo
 */

#include "util.h"

FixedScale::FixedScale(double start, std::vector<ScaleItem> items, double end) {
	this->start = start;
	this->items = items;
	this->end = end;
}

double FixedScale::value(double prog) const {
	double last = start;
	double last_prog = 0;
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i].prog >= prog) {
			return (items[i].value - last)/(items[i].prog - last_prog) * (prog - last_prog) + last;
		}

		last = items[i].value;
		last_prog = items[i].prog;
	}
	return (end - last)/(1 - last_prog) * (prog - last_prog) + last;
}

double FixedScale::progress(double value) const {
	double last = start;
	double last_prog = 0;
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i].value >= value) {
			return (value - last)/(items[i].value - last) * (items[i].prog - last_prog) + last_prog;
		}

		last = items[i].value;
		last_prog = items[i].prog;
	}
	return (value - last)/(end - last) * (1 - last_prog) + last_prog;
}
