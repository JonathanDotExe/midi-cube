/*
 * gui.cpp
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#include "gui.h"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <exception>
#include <unordered_map>


std::unordered_map<DeviceType, Texture2D> device_textures;

void load_gui_resources() {
	device_textures[DeviceType::MIDI_INPUT] = LoadTexture("./resources/images/devices/midi_input.png");
	device_textures[DeviceType::MIDI_OUTPUT] = LoadTexture("./resources/images/devices/midi_output.png");
	device_textures[DeviceType::SOUND_ENGINE] = LoadTexture("./resources/images/devices/sound_engine.png");
}

//View
GUIModel* View::get_model() {
	return model;
}

void View::init_model(GUIModel* model) {
	if (this->model) {
		throw std::runtime_error("Model already set");
	}
	else  {
		this->model = model;
	}
}

//Frame
Frame::Frame() {
	view = nullptr;
}

void Frame::run () {
	//Init
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MIDICube - universal MIDI and synthesis workstation");
	SetTargetFPS(60);
	//Loop
	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			change_view(view->draw());
		EndDrawing();
	}
}

void Frame::change_view(View* view) {
	if (this->view != view) {
		delete this->view;
		this->view = view;
	}
}

Frame::~Frame() {
	delete view;
	view = nullptr;
}

//MainMenuView
View* MainMenuView::draw() {
	View* view = this;
	//Title
	int titleWidth = MeasureText("MIDICube", 72);
	DrawText("MIDICube", SCREEN_WIDTH/2 - titleWidth/2, 30, 72, BLACK);

	//Button
	if (GuiButton({SCREEN_WIDTH/2 - 200, 160, 400, 40}, "Devices")) {
		view = new DevicesMenuView();
	}
	return view;
}

//DevicesMenuView
View* DevicesMenuView::draw() {
	View* view = this;

	//Draw devices
	for (auto device : get_model()->midi_cube->get_devices()) {
		Texture2D tex = device_textures[device.second->type()];

	}

	return view;
}

