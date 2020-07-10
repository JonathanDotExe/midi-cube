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
#include <algorithm>


std::unordered_map<DeviceType, Texture2D> device_textures;

void load_gui_resources() {
	device_textures[DeviceType::MIDI_OUTPUT] = LoadTexture("./resources/images/devices/midi_output.png");
	device_textures[DeviceType::MIDI_INPUT] = LoadTexture("./resources/images/devices/midi_input.png");
	device_textures[DeviceType::SOUND_ENGINE] = LoadTexture("./resources/images/devices/sound_engine.png");
}

void unload_gui_ressources() {
	for (auto tex : device_textures) {
		UnloadTexture(tex.second);
	}
	device_textures.clear();
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
	//Load ressources
	if (!device_textures.size()) {
		load_gui_resources();
	}
	//Loop
	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			change_view(view->draw());
		EndDrawing();
	}
	//Cleanup
	unload_gui_ressources();
	CloseWindow();
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
		view->init_model(get_model());
	}
	return view;
}

//DevicesMenuView
View* DevicesMenuView::draw() {
	View* view = this;
	GUIModel& model = *get_model();
	//Draw devices
	for (auto device : model.midi_cube->get_devices()) {
		Position* pos = model.get_position(device.first);
		Texture2D tex = device_textures[device.second->type()];
		//Texture
		DrawTexture(tex, pos->x, pos->y, WHITE);
		//Name
		DrawText(device.first.c_str(), pos->x, pos->y + tex.height + 1, 4, BLACK);
	}
	//Move devices
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		Vector2 mouse_pos = GetMousePosition();
		//Old click
		if (device_drag.dragging) {
			Position& pos = *model.device_positions[device_drag.device];
			Texture2D tex = device_textures[model.midi_cube->get_devices()[device_drag.device]->type()];
			pos.x += mouse_pos.x - device_drag.last_x;
			pos.y += mouse_pos.y - device_drag.last_y;
			pos.x = std::min(std::max(pos.x, 0), SCREEN_WIDTH - tex.width);
			pos.y = std::min(std::max(pos.y, 0), SCREEN_HEIGHT - tex.height);
		}
		else {
			//Find position
			for (auto p : model.device_positions) {
				Position& pos = *p.second;
				Texture2D tex = device_textures[model.midi_cube->get_devices()[p.first]->type()];
				if (mouse_pos.x >= pos.x && mouse_pos.x <= pos.x + tex.width && mouse_pos.y >= pos.y && mouse_pos.y <= pos.y + tex.height) {
					//Clicked
					device_drag.dragging = true;
					device_drag.device = p.first;
				}
			}
		}
		device_drag.last_x = mouse_pos.x;
		device_drag.last_y = mouse_pos.y;
	}
	else {
		device_drag.dragging = false;
	}

	return view;
}

