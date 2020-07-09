/*
 * model.cpp
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#include "model.h"

GUIModel::GUIModel() {
	midiCube = nullptr;
}

Position* GUIModel::get_position(std::string device) {
	if (device_positions.find(device) != device_positions.end()) {
		device_positions[device] = new Position();
	}
	return device_positions[device];
}

GUIModel::~GUIModel() {
	for (auto dev : device_positions) {
		delete dev.second;
	}
	device_positions.clear();
}

