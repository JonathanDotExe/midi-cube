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
#include <sstream>


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
	//Bindings
	for (DeviceBinding binding : model.midi_cube->get_bindings()) {
		Position* pos1 = model.get_position(binding.input);
		AudioDevice* device1 = model.midi_cube->get_devices()[binding.input];
		Texture2D tex1 = device_textures[device1->type()];

		Position* pos2= model.get_position(binding.output);
		AudioDevice* device2 = model.midi_cube->get_devices()[binding.output];
		Texture2D tex2 = device_textures[device2->type()];

		//Lines
		Vector2 start;
		Vector2 end;

		start.x = pos1->x + tex1.width/2;
		start.y = pos1->y + tex1.height/2;

		end.x = pos2->x + tex2.width/2;
		end.y = pos2->y + tex2.height/2;

		DrawLineEx(start, end, 2.0f, RED);

		//Channels
		Vector2 text_pos;
		text_pos.x = (start.x + end.x) / 2;
		text_pos.y = (start.y + end.y) / 2 - 20;
		std::ostringstream text;
		text << "Channel " << binding.input_channel << " - Channel " << binding.output_channel;
		DrawText(text.str().c_str(), text_pos.x, text_pos.y, 4, BLACK);
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

