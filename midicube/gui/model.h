/*
 * model.h
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_MODEL_H_
#define MIDICUBE_GUI_MODEL_H_

#include <unordered_map>
#include <string>
#include "../midicube.h"

struct Position {
	int x;
	int y;
};

class GUIModel {
public:
	MidiCube* midiCube;
	std::unordered_map<std::string, Position*> device_positions;

	GUIModel();

	Position* get_position(std::string device);

	~GUIModel();
};

#endif /* MIDICUBE_GUI_MODEL_H_ */
