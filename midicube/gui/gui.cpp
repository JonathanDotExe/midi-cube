/*
 * gui.cpp
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#include "gui.h"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
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
	//Load resources
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
		if (!view) {
			if (history.size() > 0) {
				delete this->view;
				this->view = history.back();
				history.pop_back();
			}
		}
		else {
			if (this->view && this->view->save_to_history()) {
				history.push_back(this->view);
			}
			if (this->view && !this->view->save_to_history()) {
				delete this->view;
			}
			this->view = view;
		}
	}
}

Frame::~Frame() {
	delete view;
	view = nullptr;

	for (size_t i = 0; i < history.size(); ++i) {
		delete history[i];
	}
	history.clear();
}

//GUI utils
static void draw_return_button (View** view) {
	if (GuiButton({SCREEN_WIDTH - 30, SCREEN_HEIGHT - 30, 30, 30}, GuiIconText(RICON_REREDO_FILL, ""))) {
		*view = nullptr;
	}
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
DevicesMenuView::DevicesMenuView() {
	binding_drag = {};
	device_drag = {};
}

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
		start.y = pos1->y + tex1.height/2 + binding.input_channel * 2 - 16;

		end.x = pos2->x + tex2.width/2;
		end.y = pos2->y + tex2.height/2 + binding.output_channel * 2 - 16;

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
	Vector2 mouse_pos = GetMousePosition();
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		if (!binding_drag.dialog) {
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
						break;
					}
				}
				device_drag.start_x = mouse_pos.x;
				device_drag.start_y = mouse_pos.y;
			}
			device_drag.last_x = mouse_pos.x;
			device_drag.last_y = mouse_pos.y;
		}
	}
	else {
		//Edit device
		if (device_drag.dragging && device_drag.start_x == mouse_pos.x && device_drag.start_y == mouse_pos.y) {
			view = create_view_for_device(model.midi_cube->get_devices().at(device_drag.device));
			if (!view) {
				view = this;
			}
		}
		device_drag.dragging = false;
	}
	//Bind devices
	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
		if (binding_drag.dragging) {
			//Draw
			Position* pos = model.get_position(binding_drag.input_device);
			AudioDevice* device = model.midi_cube->get_devices()[binding_drag.input_device];
			Texture2D tex = device_textures[device->type()];

			Vector2 start;

			start.x = pos->x + tex.width/2;
			start.y = pos->y + tex.height/2;

			DrawLineEx(start, mouse_pos, 2.0f, BLUE);
		}
		else {
			//Find position
			for (auto p : model.device_positions) {
				Position& pos = *p.second;
				Texture2D tex = device_textures[model.midi_cube->get_devices()[p.first]->type()];
				if (mouse_pos.x >= pos.x && mouse_pos.x <= pos.x + tex.width && mouse_pos.y >= pos.y && mouse_pos.y <= pos.y + tex.height) {
					//Clicked
					binding_drag.dragging = true;
					binding_drag.input_device = p.first;
					break;
				}
			}
		}
	}
	else {
		if (binding_drag.dragging) {
			//Find position
			for (auto p : model.device_positions) {
				Position& pos = *p.second;
				Texture2D tex = device_textures[model.midi_cube->get_devices()[p.first]->type()];
				if (p.first != binding_drag.input_device && mouse_pos.x >= pos.x && mouse_pos.x <= pos.x + tex.width && mouse_pos.y >= pos.y && mouse_pos.y <= pos.y + tex.height) {
					//Clicked
					binding_drag.dialog = true;
					binding_drag.output_device = p.first;
					break;
				}
			}
			binding_drag.dragging = false;
		}
	}

	//Show channel dialog
	if (binding_drag.dialog) {
		const int input_width = 300;
		const int input_height = 40;
		const int text_height = 16;
		const int box_width = input_width  + 10;
		const int box_height = 3 * input_height + 5 + 2 * text_height + 10;
		//Background
		DrawRectangle(SCREEN_WIDTH/2 - box_width/2, SCREEN_HEIGHT/2 - box_height/2, box_width, box_height, GRAY);
		//Components
		int pos_y = SCREEN_HEIGHT/2 - box_height/2 + 5;
		DrawText("Input Channel: ", SCREEN_WIDTH/2 - input_width/2, pos_y + 2, 4, BLACK);
		pos_y += text_height;
		Rectangle rect;
		rect.x = SCREEN_WIDTH/2 - input_width/2;
		rect.y = pos_y;
		rect.width = input_width;
		rect.height = input_height;
		GuiSpinner(rect, "Input Channel: ", &binding_drag.input_channel, -1, 15, false);
		pos_y += input_height;
		DrawText("Output Channel: ", SCREEN_WIDTH/2 - input_width/2, pos_y + 2, 4, BLACK);
		pos_y += text_height;
		rect.y = pos_y;
		GuiSpinner(rect, "Output Channel: ", &binding_drag.output_channel, -1, 15, false);
		pos_y += input_height + 5;
		rect.y = pos_y;
		if (GuiButton(rect, "Create Binding")) {
			binding_drag.dialog = false;
			DeviceBinding binding;
			binding.input = binding_drag.input_device;
			binding.input_channel = binding_drag.input_channel;
			binding.output = binding_drag.output_device;
			binding.output_channel = binding_drag.output_channel;
			model.midi_cube->add_binding(binding);
		}
	}
	draw_return_button(&view);

	return view;
}

SoundEngineDeviceMenuView::SoundEngineDeviceMenuView(SoundEngineDevice* device) {
	this->device = device;
	//Options
	std::ostringstream optionstr;
	std::vector<SoundEngine*> engines = device->get_sound_engines();
	optionstr << "None";
	for (SoundEngine* engine : engines) {
		optionstr << ";" << engine->get_name();
	}
	options = optionstr.str();
}

View* SoundEngineDeviceMenuView::draw() {
	View* view = this;
	std::vector<SoundEngine*> engines = device->get_sound_engines();
	//Engines
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		unsigned int ch = SOUND_ENGINE_MIDI_CHANNELS - 1 - i;
		SoundEngineChannel& channel = device->get_channel(ch);
		int selected = channel.get_engine_index(engines) + 1;
		int old = selected;
		float y = 5 + ch * 25;
		//Text
		std::ostringstream text;
		text << "Channel " << ch;
		DrawText(text.str().c_str(), 5, y + 6, 4, BLACK);
		//ComboBox
		selected = GuiComboBox((Rectangle){100, y, 400, 20}, options.c_str(), selected);
		if (old != selected) {
			channel.set_engine(selected > 0 ? engines.at(selected - 1) : nullptr);
		}
		//Volume Slider
		channel.volume = GuiSlider((Rectangle){600, y, 300, 20}, "Vol ", TextFormat("%1.2f", channel.volume.load()), channel.volume, 0, 1);
	}

	draw_return_button(&view);

	return view;
}

View* create_view_for_device(AudioDevice* device) {
	if (device->get_identifier() == "Sound Engine") { //TODO cleaner check
		return new SoundEngineDeviceMenuView(dynamic_cast<SoundEngineDevice*>(device));
	}
	return nullptr;
}


