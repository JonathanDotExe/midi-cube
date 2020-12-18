/*
 * gui.h
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_GUI_H_
#define MIDICUBE_GUI_GUI_H_

#include "../util.h"
#include "../midicube.h"
#include "../soundengine/organ.h"
#include "../soundengine/soundengine.h"
#include <type_traits>

#include "engine/core.h"
#include "engine/control.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

class Dialog {
public:
	virtual bool draw(float x, float y) = 0;
	virtual float width() = 0;
	virtual float height() = 0;
	virtual ~Dialog() {

	}
};

class MainMenuView : public ViewController {

private:
	MidiCube* cube;

public:

	MainMenuView(MidiCube* cube);

	Node* init(Frame* frame);

};

class SoundEngineMenuView : public ViewController {

private:
	MidiCube* cube;

public:

	SoundEngineMenuView(MidiCube* cube);

	Node* init(Frame* frame);

};

class B3OrganMenuView : public ViewController {

private:
	B3OrganData* data;
	MidiCube* cube;

public:

	B3OrganMenuView(B3OrganData* data, MidiCube* cube);

	Node* init(Frame* frame);

};

ViewController* create_view_for_engine(std::string engine_name, SoundEngine* engine, MidiCube* cube);

#endif /* MIDICUBE_GUI_GUI_H_ */
