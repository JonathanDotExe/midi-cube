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
#include <iostream>


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
	SetTargetFPS(30);
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

static bool draw_switch (int x, int y, int width, int height, bool value, std::string on_text = "ON", std::string off_text = "OFF") {
	DrawRectangle(x, y, width, height, RAYWHITE);

	//Click
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 point = GetMousePosition();
		if (point.x >= x && point.x < x + width && point.y >= y && point.y < y + height) {
			value = !value;
		}
	}

	//Switch
	DrawRectangle(x, y + height/2 * (value ? 0 : 1) , width, height/2, GRAY);
	std::string text = value ? on_text : off_text;
	int text_width = MeasureText(text.c_str(), 8);
	if (value) {
		DrawText(text.c_str(), x + width/2 - text_width/2, y + 2, 8, BLACK);
	}
	else {
		DrawText(text.c_str(), x + width/2 - text_width/2, y + height - 10, 8, BLACK);
	}

	return value;
}

/*
static void scaled_slider(Rectangle pos, std::string text, double& value, const FixedScale& scale, std::string format = "1.2f%") {
	value = scale.value(GuiSlider(pos, text.c_str(), TextFormat(format.c_str(), value), scale.progress(value), 0, 1));
}*/

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
		if (!binding_drag.dialog && !drag_binding) {
			if (device_drag.dragging) {
				Position& pos = *model.device_positions[device_drag.device];
				Texture2D tex = device_textures[model.midi_cube->get_devices()[device_drag.device]->type()];
				pos.x += mouse_pos.x - device_drag.last_x;
				pos.y += mouse_pos.y - device_drag.last_y;
				pos.x = std::min(std::max(pos.x, 0), SCREEN_WIDTH - tex.width);
				pos.y = std::min(std::max(pos.y, 0), SCREEN_HEIGHT - tex.height);

				if (device_drag.dragging && GetTime() > press_time + 1 && device_drag.start_x == mouse_pos.x && device_drag.start_y == mouse_pos.y) {
					drag_binding = true;
				}
			}
			else {
				press_time = GetTime();
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
		drag_binding = true;
	}
	drag_binding = drag_binding && (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON));
	if (drag_binding) {
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
					binding_drag.input_channel = -1;
					binding_drag.output_channel = -1;
					binding_drag.start_note = 0;
					binding_drag.end_note = 127;
					binding_drag.octave = 0;
					binding_drag.transfer_cc = true;
					binding_drag.transfer_channel_aftertouch = true;
					binding_drag.transfer_other = true;
					binding_drag.transfer_pitch_bend = true;
					binding_drag.transfer_prog_change = true;

					binding_drag.input_channel_editmode = false;
					binding_drag.output_channel_editmode = false;
					binding_drag.start_note_editmode = false;
					binding_drag.end_note_editmode = false;
					binding_drag.octave_editmode = false;
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
		const int check_height = 20;
		const int text_height = 16;
		const int box_width = input_width  + 10;
		const int box_height = 6 * input_height + 5 + 5 * text_height + (check_height + 5) * 3 + 10;
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
		if (GuiSpinner(rect, "Input Channel: ", &binding_drag.input_channel, -1, 15, binding_drag.input_channel_editmode)) {
			binding_drag.input_channel_editmode = !binding_drag.input_channel_editmode;
		}
		pos_y += input_height;

		DrawText("Output Channel: ", SCREEN_WIDTH/2 - input_width/2, pos_y + 2, 4, BLACK);
		pos_y += text_height;
		rect.y = pos_y;
		if (GuiSpinner(rect, "Output Channel: ", &binding_drag.output_channel, -1, 15, binding_drag.output_channel_editmode))  {
			binding_drag.output_channel_editmode = !binding_drag.output_channel_editmode;
		}
		pos_y += input_height;

		DrawText("Start Note: ", SCREEN_WIDTH/2 - input_width/2, pos_y + 2, 4, BLACK);
		pos_y += text_height;
		rect.y = pos_y;
		if (GuiSpinner(rect, "Start Note: ", &binding_drag.start_note, 0, 127, binding_drag.start_note_editmode)) {
			binding_drag.start_note_editmode = !binding_drag.start_note_editmode;
		}
		pos_y += input_height;

		DrawText("End Note: ", SCREEN_WIDTH/2 - input_width/2, pos_y + 2, 4, BLACK);
		pos_y += text_height;
		rect.y = pos_y;
		if (GuiSpinner(rect, "End Note: ", &binding_drag.end_note, 0, 127, binding_drag.end_note_editmode)) {
			binding_drag.end_note_editmode = !binding_drag.end_note_editmode;
		}
		pos_y += input_height;

		DrawText("Octave: ", SCREEN_WIDTH/2 - input_width/2, pos_y + 2, 4, BLACK);
		pos_y += text_height;
		rect.y = pos_y;
		if (GuiSpinner(rect, "Octave: ", &binding_drag.octave, -4, 4, binding_drag.octave_editmode)) {
			binding_drag.octave_editmode = !binding_drag.octave_editmode;
		}
		pos_y += input_height + 5;

		rect.height = check_height;
		rect.width = check_height;
		rect.y = pos_y;
		binding_drag.transfer_cc = GuiCheckBox(rect, "CC", binding_drag.transfer_cc);

		rect.x = SCREEN_WIDTH/2;
		binding_drag.transfer_prog_change = GuiCheckBox(rect, "Prog.", binding_drag.transfer_prog_change);
		pos_y += check_height + 5;

		rect.x = SCREEN_WIDTH/2 - input_width/2;
		rect.y = pos_y;
		binding_drag.transfer_pitch_bend = GuiCheckBox(rect, "Pitch Bend", binding_drag.transfer_pitch_bend);

		rect.x = SCREEN_WIDTH/2;
		binding_drag.transfer_channel_aftertouch = GuiCheckBox(rect, "Channel Aftertouch", binding_drag.transfer_channel_aftertouch);
		pos_y += check_height + 5;

		rect.x = SCREEN_WIDTH/2 - input_width/2;
		rect.y = pos_y;
		binding_drag.transfer_other = GuiCheckBox(rect, "SYSEX", binding_drag.transfer_other);
		pos_y += check_height + 5;

		rect.height = input_height;
		rect.width = input_width;
		rect.y = pos_y;
		if (GuiButton(rect, "Create Binding")) {
			binding_drag.dialog = false;
			DeviceBinding binding;
			binding.input = binding_drag.input_device;
			binding.input_channel = binding_drag.input_channel;
			binding.output = binding_drag.output_device;
			binding.output_channel = binding_drag.output_channel;
			binding.octave = binding_drag.octave;

			binding.start_note = binding_drag.start_note;
			binding.end_note = binding_drag.end_note;
			binding.transfer_cc = binding_drag.transfer_cc;
			binding.transfer_channel_aftertouch = binding_drag.transfer_channel_aftertouch;
			binding.transfer_other = binding_drag.transfer_other;
			binding.transfer_pitch_bend = binding_drag.transfer_pitch_bend;
			binding.transfer_prog_change = binding_drag.transfer_prog_change;

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
	std::vector<SoundEngineBank*> engines = device->get_sound_engines();
	optionstr << "None";
	for (SoundEngineBank* engine : engines) {
		optionstr << ";" << engine->get_name();
	}
	options = optionstr.str();
}

View* SoundEngineDeviceMenuView::draw() {
	View* view = this;
	std::vector<SoundEngineBank*> engines = device->get_sound_engines();
	//Engines
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		unsigned int ch = SOUND_ENGINE_MIDI_CHANNELS - 1 - i;
		SoundEngineChannel& channel = device->get_channel(ch);
		int selected = channel.get_engine() + 1;
		int old = selected;
		float y = 5 + ch * 25;
		//Text
		std::ostringstream text;
		text << "Channel " << ch;
		DrawText(text.str().c_str(), 5, y + 6, 4, BLACK);
		//ComboBox
		selected = GuiComboBox((Rectangle){100, y, 250, 20}, options.c_str(), selected);
		if (old != selected) {
			channel.set_engine(selected - 1);
		}
		//Edit
		if (GuiButton((Rectangle){360, y, 20, 20}, GuiIconText(RICON_PENCIL, "")) && selected > 0) {
			view = create_view_for_engine(engines.at(selected - 1)->get_name(), channel.get_engine(engines, ch)); //TODO Thread safety for data
			if (!view) {
				view = this;
			}
		}
		//Volume Slider
		channel.volume = GuiSlider((Rectangle){420, y, 200, 20}, "Vol", TextFormat("%1.2f", channel.volume.load()), channel.volume, 0, 1);

		//Active and solo
		channel.active = GuiCheckBox((Rectangle){650, y, 20, 20}, "A", channel.active);
		if (GuiButton((Rectangle){690, y, 20, 20}, "S")) {
			device->solo(ch);
		}

		//Looper
		channel.get_looper().preset.on = GuiCheckBox((Rectangle){730, y, 20, 20}, "Looper", channel.get_looper().preset.on);
		if (GuiButton((Rectangle){810, y, 50, 20}, "Clear")) {
			channel.get_looper().reset = true;
		}
		//Bars
		int bars = channel.get_looper().preset.bars;
		GuiSpinner((Rectangle){870, y, 80, 20}, "", &bars, 1, 16, false);
		channel.get_looper().preset.bars = bars;
		//Play and Record
		channel.get_looper().play = GuiCheckBox((Rectangle){950, y, 20, 20}, "P", channel.get_looper().play);
		channel.get_looper().record = GuiCheckBox((Rectangle){985, y, 20, 20}, "R", channel.get_looper().record);
	}

	//Metronome
	//Play
	device->play_metronome = GuiCheckBox((Rectangle){10, SCREEN_HEIGHT - 35, 20, 20}, "Metronome", device->play_metronome);
	//BPM
	int bpm = device->metronome.get_bpm();
	if (GuiSpinner((Rectangle){150, SCREEN_HEIGHT - 40, 150, 30}, "BPM", &bpm, 10, 1920, bpm_editmode)) {
		bpm_editmode = !bpm_editmode;
	}
	device->metronome.set_bpm(bpm);

	draw_return_button(&view);

	return view;
}

View* create_view_for_device(AudioDevice* device) {
	if (device->get_identifier() == "Sound Engine") { //TODO cleaner check
		return new SoundEngineDeviceMenuView(dynamic_cast<SoundEngineDevice*>(device));
	}
	return nullptr;
}

//B3OrganEngineView
void B3OrganEngineMenuView::draw_drawbar (int x, int y, int width, int height, size_t index) {
	std::array<std::atomic<unsigned int>, ORGAN_DRAWBAR_COUNT>& drawbars = data->preset.drawbars;
	//Move drawbar
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		int count = GetTouchPointsCount();
		for (int i = 0; i < count; ++i) {
			Vector2 point = GetTouchPosition(i);
			if (point.x >= x && point.x < x + width && point.y >= y && point.y < y + height) {
				int val = 9.0 * (point.y - y)/height;
				drawbars.at(index) = std::min(val, ORGAN_DRAWBAR_MAX);	//TODO thread-safety
			}
		}
	}
	//Draw caption
	int title_width = MeasureText(drawbar_titles.at(index).c_str(), 16);
	DrawText(drawbar_titles.at(index).c_str(), x + width/2 - title_width/2, y - 14, 12, YELLOW);
	//Draw drawbar
	int val = drawbars.at(index);
	Color color = WHITE;
	if (index <= 1) {
		color = RED;
	}
	else if (index == 4 || index == 6 || index == 7) {
		color = BLACK;
	}
	DrawRectangle(x + 5, y, width - 10 , height, RAYWHITE);
	DrawRectangle(x, y + height/9 * val, width, height/(ORGAN_DRAWBAR_MAX + 1), color);

	if (edit_midi) {
		//CC input
		int cc_value = data->preset.drawbar_ccs.at(index);
		Rectangle rect;
		rect.x = x;
		rect.y = y + height + 4;
		rect.width = width;
		rect.height = 20;
		DrawRectangle(rect.x, rect.y, rect.width, rect.height, RAYWHITE);
		if (GuiValueBox(rect, "", &cc_value, 0, 127, drawbar_editmode.at(index))) {
			drawbar_editmode.at(index) = !drawbar_editmode.at(index);
		}
		data->preset.drawbar_ccs.at(index) = cc_value; //TODO Thread-safety
	}
	else {
		//Draw value
		std::ostringstream valstr;
		valstr << val;
		int val_width = MeasureText(valstr.str().c_str(), 16);
		DrawText(valstr.str().c_str(), x + width/2 - val_width/2, y + height + 4, 12, BLACK);
	}
}

B3OrganEngineMenuView::B3OrganEngineMenuView(B3OrganData* data) {
	this->data = data;
}

View* B3OrganEngineMenuView::draw() {
	View* view = this;

	ClearBackground(BROWN);

	//Title
	int title_width = MeasureText("B3-Organ", 32);
	DrawText("B3-Organ", SCREEN_WIDTH/2 - title_width/2, 20, 36, BLACK);

	//Drawbars
	int width = 30;
	int space = 20;
	int total_width = width * data->preset.drawbars.size() + space * (data->preset.drawbars.size() - 1);
	for (size_t i = 0; i < data->preset.drawbars.size(); ++i) {
		draw_drawbar(SCREEN_WIDTH/2 - total_width/2 + (width + space) * i, 100, width, 200, i);
	}

	//Rotary and percussion
	if (edit_midi) {
		{
			//Rotary CC
			int cc_value = data->preset.rotary_cc;
			DrawRectangle(20, 100, 30, 30, RAYWHITE);
			if (GuiValueBox((Rectangle){20, 100, 30, 30}, "", &cc_value, 0, 127, rotary_editmode)) {
				rotary_editmode = !rotary_editmode;
			}
			data->preset.rotary_cc = cc_value;

			DrawText("Rotary Speaker", 55, 108, 12, BLACK);
		}
		{
			//Rotary Speed CC
			int cc_value = data->preset.rotary_speed_cc;
			DrawRectangle(20, 140, 30, 30, RAYWHITE);
			if (GuiValueBox((Rectangle){20, 140, 30, 30}, "", &cc_value, 0, 127, rotary_speed_editmode)) {
				rotary_speed_editmode = !rotary_speed_editmode;
			}
			data->preset.rotary_speed_cc = cc_value;

			DrawText(data->preset.rotary_fast ? "Rotary Fast" : "Rotary Slow", 55, 148, 12, BLACK);
		}
		{
			//Percussion CC
			int cc_value = data->preset.percussion_cc;
			DrawRectangle(20, 180, 30, 30, RAYWHITE);
			if (GuiValueBox((Rectangle){20, 180, 30, 30}, "", &cc_value, 0, 127, percussion_editmode)) {
				percussion_editmode = !percussion_editmode;
			}
			data->preset.percussion_cc = cc_value;

			DrawText("Percussion", 55, 188, 12, BLACK);
		}
		{
			//Percussion Third Harmonic CC
			int cc_value = data->preset.percussion_third_harmonic_cc;
			DrawRectangle(20, 220, 30, 30, RAYWHITE);
			if (GuiValueBox((Rectangle){20, 220, 30, 30}, "", &cc_value, 0, 127, percussion_third_harmonic_editmode)) {
				percussion_third_harmonic_editmode = !percussion_third_harmonic_editmode;
			}
			data->preset.percussion_third_harmonic_cc = cc_value;

			DrawText(data->preset.percussion_third_harmonic ? "Third Harmonic" : "Second Harmonic", 55, 228, 12, BLACK);
		}
		{
			//Percussion Fast Decay CC
			int cc_value = data->preset.percussion_fast_decay_cc;
			DrawRectangle(20, 260, 30, 30, RAYWHITE);
			if (GuiValueBox((Rectangle){20, 260, 30, 30}, "", &cc_value, 0, 127, percussion_fast_decay_editmode)) {
				percussion_fast_decay_editmode = !percussion_fast_decay_editmode;
			}
			data->preset.percussion_fast_decay_cc = cc_value;

			DrawText(data->preset.percussion_fast_decay ? "Fast Decay" : "Slow Decay", 55, 268, 12, BLACK);
		}
		{
			//Percussion Soft CC
			int cc_value = data->preset.percussion_soft_cc;
			DrawRectangle(20, 300, 30, 30, RAYWHITE);
			if (GuiValueBox((Rectangle){20, 300, 30, 30}, "", &cc_value, 0, 127, percussion_soft_editmode)) {
				percussion_soft_editmode = !percussion_soft_editmode;
			}
			data->preset.percussion_soft_cc = cc_value;

			DrawText(data->preset.percussion_soft ? "Soft Percussion" : "Hard Percussion", 55, 308, 12, BLACK);
		}
	}
	else {
		//Rotary speaker
		data->preset.rotary = draw_switch(20, 100, 20, 30, data->preset.rotary);
		DrawText("Rotary Speaker", 45, 108, 12, BLACK);
		data->preset.rotary_fast = draw_switch(20, 140, 20, 30, data->preset.rotary_fast, "FST", "SLW");
		DrawText(data->preset.rotary_fast ? "Rotary Fast" : "Rotary Slow", 45, 148, 12, BLACK);
		//Percussion
		data->preset.percussion = draw_switch(20, 180, 20, 30, data->preset.percussion);
		DrawText("Percussion", 45, 188, 12, BLACK);
		data->preset.percussion_third_harmonic = draw_switch(20, 220, 20, 30, data->preset.percussion_third_harmonic, "3rd", "2nd");
		DrawText(data->preset.percussion_third_harmonic ? "Third Harmonic" : "Second Harmonic", 45, 228, 12, BLACK);
		data->preset.percussion_fast_decay = draw_switch(20, 260, 20, 30, data->preset.percussion_fast_decay, "FST", "SLW");
		DrawText(data->preset.percussion_fast_decay ? "Fast Decay" : "Slow Decay", 45, 268, 12, BLACK);
		data->preset.percussion_soft = draw_switch(20, 300, 20, 30, data->preset.percussion_soft, "SFT", "HRD");
		DrawText(data->preset.percussion_soft ? "Soft Percussion" : "Hard Percussion", 45, 308, 12, BLACK);
	}

	//Modeling parameters
	DrawRectangle(18, 398, SCREEN_WIDTH/2 - 26, SCREEN_HEIGHT - 416, GRAY);
	DrawRectangle(20, 400, SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT - 420, RAYWHITE);
	data->preset.harmonic_foldback_volume = GuiSlider((Rectangle) {130, 410, 300, 20}, "Harm. Foldback Vol.", TextFormat("%1.2f", data->preset.harmonic_foldback_volume.load()), data->preset.harmonic_foldback_volume, 0, 1.0);
	data->preset.rotary_gain = GuiSlider((Rectangle) {130, 440, 300, 20}, "Rotary Gain", TextFormat("%1.2f", data->preset.rotary_gain.load()), data->preset.rotary_gain, 0, 2.0);
	data->preset.rotary_stereo_mix = GuiSlider((Rectangle) {130, 470, 300, 20}, "Rotary Stereo Mix", TextFormat("%1.2f", data->preset.rotary_stereo_mix.load()), data->preset.rotary_stereo_mix, 0, 1.0);
	data->preset.rotary_delay = GuiSlider((Rectangle) {130, 500, 300, 20}, "Rotary Delay", TextFormat("%1.2f ms", data->preset.rotary_delay.load() * 1000), data->preset.rotary_delay, 0, 0.005);
	data->preset.rotary_type = GuiCheckBox((Rectangle) {130, 530, 20, 20}, data->preset.rotary_type ? "Rotary Type 2" : "Rotary Type 1", data->preset.rotary_type);
	//Amplifier
	float x = SCREEN_WIDTH/2 + 120;
	if (edit_midi) {
		DrawRectangle(SCREEN_WIDTH/2 + 8, 398, SCREEN_WIDTH/2 - 26, SCREEN_HEIGHT - 416, GRAY);
		DrawRectangle(SCREEN_WIDTH/2 + 10, 400, SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT - 420, GOLD);
		data->preset.overdrive = GuiSlider((Rectangle) {x, 420, 300, 20}, "Overdrive", TextFormat("%1.2f", data->preset.overdrive.load()), data->preset.overdrive, 0, 1.0);

		{
			//Overdrive Gain CC
			int cc_value = data->preset.overdrive_cc;
			DrawRectangle(SCREEN_WIDTH - 60, 420, 30, 20, RAYWHITE);
			if (GuiValueBox((Rectangle){SCREEN_WIDTH - 60, 420, 30, 20}, "", &cc_value, 0, 127, overdrive_editmode)) {
				overdrive_editmode = !overdrive_editmode;
			}
			data->preset.overdrive_cc = cc_value;
		}
	}
	else {
		DrawRectangle(SCREEN_WIDTH/2 + 8, 398, SCREEN_WIDTH/2 - 26, SCREEN_HEIGHT - 416, GRAY);
		DrawRectangle(SCREEN_WIDTH/2 + 10, 400, SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT - 420, GOLD);
		data->preset.overdrive = GuiSlider((Rectangle) {x, 400, 300, 20}, "Overdrive", TextFormat("%1.2f", data->preset.overdrive.load()), data->preset.overdrive, 0, 1.0);
	}
	std::vector<DistortionType> types{DistortionType::DIGITAL, DistortionType::ANALOG_1, DistortionType::ANALOG_2};
	int type = std::find(types.begin(), types.end(), data->preset.distortion_type.load()) - types.begin();
	GuiSpinner((Rectangle) {x, 430, 150, 20}, "Type", &type, 0, types.size() - 1, false);
	data->preset.normalize_overdrive = GuiCheckBox((Rectangle) {x + 170, 430, 20, 20}, "Normalize", data->preset.normalize_overdrive);
	if (type >= 0 && (size_t) type < types.size()) {
		data->preset.distortion_type = types[type];
	}

	//Edit MIDI
	if (GuiButton({SCREEN_WIDTH - 60, SCREEN_HEIGHT - 30, 30, 30}, GuiIconText(RICON_PENCIL, ""))) {
		edit_midi = !edit_midi;
	}


	draw_return_button(&view);
	return view;
}
/*
SynthesizerEngineMenuView::SynthesizerEngineMenuView(SynthesizerData* data) {
	this->data = data;
}

View* SynthesizerEngineMenuView::draw() {
	View* view = this;

	//Title
	int title_width = MeasureText("Synthesizer", 32);
	DrawText("Synthesizer", SCREEN_WIDTH/2 - title_width/2, 20, 36, BLACK);

	//Grid
	int length = sqrt(MAX_COMPONENTS);
	for (size_t i = 0; i < data->preset.components.size(); ++i) {
		ComponentSlot& comp = data->preset.components[i];
		int x = i % length;
		int y = i / length;
		//Click
		if (GuiButton((Rectangle){20.0f + 90 * x, 70.0f + 90 * y, 80, 80}, "")) {
			if (comp.get_component()) {
				set_comp_dialog(create_dialog_for_component(comp.get_component()->get_name(), comp.get_component()));
			}
			else {
				set_comp_dialog(nullptr);
			}
			set_slot_dialog(new ComponentSlotDialog(&comp));
		}
		Color c(comp.get_component() ? BLUE : GRAY);
		DrawRectangle(20 + 90 * x, 70 + 90 * y, 80, 80, c);
		if (comp.get_component()) {
			double twidth = MeasureText(comp.get_component()->get_name().c_str(), 10);
			DrawText(comp.get_component()->get_name().c_str(), 20 + 90 * x + 40 - twidth/2, 70 + 90 * y + 2, 10, BLACK);
			std::vector<std::string> desc = comp.get_component()->get_description();
			for (size_t j = 0; j < desc.size(); ++j) {
				DrawText(desc[j].c_str(), 20 + 90 * x + 3, 70 + 90 * y + 15 + j * 10, 8, BLACK);
			}
			DrawText(std::to_string(i).c_str(), 20 + 90 * x + 73, 70 + 90 * y + 70, 8, WHITE);
		}
	}

	//Mono
	data->preset.mono = GuiCheckBox((Rectangle){20, 520, 20, 20}, "Mono", data->preset.mono);
	data->preset.portamendo_time = GuiSlider((Rectangle){150, 520, 250, 20}, "Portamendo", TextFormat("%1.4f", data->preset.portamendo_time), data->preset.portamendo_time, 0, 1);

	//Preset Input
	Rectangle rect;
	rect.x = 70;
	rect.y = SCREEN_HEIGHT - 20;
	rect.width = 100;
	rect.height = 20;

	GuiSpinner(rect, "Preset No.", &current_preset, 0, 9, false);

	//Preset Button
	rect.x += 100;
	if (GuiButton(rect, "Apply")) {
		set_comp_dialog(nullptr);
		data->update_preset = current_preset;
	}

	//Dialog
	double y = 70;
	if (comp_dialog) {
		double width = comp_dialog->width();
		double height = comp_dialog->height();
		double x = SCREEN_WIDTH - width - 20;
		if (comp_dialog->draw(x, y)) {
			set_comp_dialog(nullptr);
			set_slot_dialog(nullptr);
		}
		y += height + 20;
	}
	if (slot_dialog) {
		double width = slot_dialog->width();
		double x = SCREEN_WIDTH - width - 20;
		if (slot_dialog->draw(x, y)) {
			set_comp_dialog(nullptr);
			set_slot_dialog(nullptr);
		}
	}

	draw_return_button(&view);
	return view;
}

void SynthesizerEngineMenuView::set_comp_dialog(Dialog* d) {
	delete comp_dialog;
	comp_dialog = d;
}

void SynthesizerEngineMenuView::set_slot_dialog(Dialog* d) {
	delete slot_dialog;
	slot_dialog = d;
}

SynthesizerEngineMenuView::~SynthesizerEngineMenuView() {
	set_comp_dialog(nullptr);
	set_slot_dialog(nullptr);
}

//OscilatorDialog
OscilatorDialog::OscilatorDialog(OscilatorComponent* osc) {
	this->osc = osc;
}

bool OscilatorDialog::draw(float x, float y) {
	//Waveform
	std::string options = "SINE;SAW DOWN;SAW UP;SQUARE;NOISE";
	std::vector<AnalogWaveForm> waveforms = {AnalogWaveForm::SINE, AnalogWaveForm::SAW_DOWN, AnalogWaveForm::SAW_UP, AnalogWaveForm::SQUARE, AnalogWaveForm::NOISE};
	int waveform = std::find(waveforms.begin(), waveforms.end(), osc->osc.data.waveform) - waveforms.begin();
	DrawText("Waveform", x, y, 12, BLACK);
	y += 15;
	waveform = GuiComboBox((Rectangle){x, y, 400, 20}, options.c_str(), waveform);
	osc->osc.data.waveform = waveforms.at(waveform);
	y += 25;
	//Volume
	osc->volume = GuiSlider((Rectangle){x + 40, y, 320, 20}, "Vol.", TextFormat("%1.2f", osc->volume), osc->volume, 0, 1);
	y += 25;
	//Unison
	int unison = osc->osc.unison_amount;
	GuiSpinner((Rectangle){x + 100, y, 90, 20}, "Unison", &unison, 0, 7, false); //TODO use value of template
	osc->osc.unison_amount = unison;
	osc->unison_detune = GuiSlider((Rectangle){x + 240, y, 120, 20}, "Det.", TextFormat("%1.2f", osc->unison_detune), osc->unison_detune, 0, 1);
	y += 25;
	//Semi
	int semi = osc->semi;
	GuiSpinner((Rectangle){x + 100, y, 90, 20}, "Semi", &semi, -48, 48, false); //TODO use value of template
	double fine = osc->semi - semi;
	fine = GuiSlider((Rectangle){x + 240, y, 120, 20}, "Fine", TextFormat("%1.2f", fine), fine, 0, 0.99999f);
	osc->semi = semi + fine;
	y += 25;
	//Transpose
	if (GuiButton((Rectangle){x + 20, y, 20, 20}, "")) {
		osc->transpose = 1;
	}
	osc->transpose = GuiSlider((Rectangle){x + 40, y, 320, 20}, "F", TextFormat("%1.2f", osc->transpose), osc->transpose, 0, 25);
	y += 25;
	//Pulse width
	osc->pulse_width = GuiSlider((Rectangle){x + 40, y, 320, 20}, "Pulse Width", TextFormat("%1.2f", osc->pulse_width), osc->pulse_width, 0.1, 0.5);
	y += 25;
	//Analog and sync
	osc->osc.data.analog = GuiCheckBox((Rectangle){x, y, 20, 20}, "Analog", osc->osc.data.analog);
	osc->osc.data.sync = GuiCheckBox((Rectangle){x + 200, y, 20, 20}, "Sync", osc->osc.data.sync);
	y += 25;
	//Sync amount
	osc->sync = GuiSlider((Rectangle){x + 80, y, 280, 20}, "Sync", TextFormat("%1.2f", osc->sync), osc->sync, 1, 10);
	y += 25;
	//Reset and randomize
	osc->reset = GuiCheckBox((Rectangle){x, y, 20, 20}, "Reset", osc->reset);
	osc->randomize = GuiCheckBox((Rectangle){x + 200, y, 20, 20}, "Randomize", osc->randomize);
	y += 25;
	//Modulator
	osc->modulator = GuiCheckBox((Rectangle){x, y, 20, 20}, "Modulator", osc->modulator);
	y += 25;
	return false;
}
float OscilatorDialog::width() {
	return 400;
}
float OscilatorDialog::height() {
	return 240;
}

//AmpEnvelopeDialog
AmpEnvelopeDialog::AmpEnvelopeDialog(AmpEnvelopeComponent* amp) {
	this->amp = amp;
}

bool AmpEnvelopeDialog::draw(float x, float y) {
	scaled_slider((Rectangle){x + 20, y, 320, 20}, "A", amp->envelope.attack, ATTACK_SCALE, "%1.4f");
	y += 25;
	scaled_slider((Rectangle){x + 20, y, 320, 20}, "D", amp->envelope.decay, DECAY_SCALE, "%1.4f");
	y += 25;
	amp->envelope.sustain = GuiSlider((Rectangle){x + 20, y, 320, 20}, "S", TextFormat("%1.4f", amp->envelope.sustain), amp->envelope.sustain, 0, 1);
	y += 25;
	scaled_slider((Rectangle){x + 20, y, 320, 20}, "R", amp->envelope.release, RELEASE_SCALE, "%1.4f");
	y += 25;
	amp->amplitude = GuiSlider((Rectangle){x + 20, y, 320, 20}, "Vol.", TextFormat("%1.2f", amp->amplitude), amp->amplitude, 0, 1);
	y += 25;
	return false;
}

float AmpEnvelopeDialog::width() {
	return 400;
}

float AmpEnvelopeDialog::height() {
	return 120;
}

//ModEnvelopeDialog
ModEnvelopeDialog::ModEnvelopeDialog(ModEnvelopeComponent* env) {
	this->env = env;
}

bool ModEnvelopeDialog::draw(float x, float y) {
	scaled_slider((Rectangle){x + 20, y, 320, 20}, "A", env->envelope.attack, ATTACK_SCALE, "%1.4f");
	y += 25;
	scaled_slider((Rectangle){x + 20, y, 320, 20}, "D", env->envelope.decay, DECAY_SCALE, "%1.4f");
	y += 25;
	env->envelope.sustain = GuiSlider((Rectangle){x + 20, y, 320, 20}, "S", TextFormat("%1.4f", env->envelope.sustain), env->envelope.sustain, 0, 1);
	y += 25;
	scaled_slider((Rectangle){x + 20, y, 320, 20}, "R", env->envelope.release, RELEASE_SCALE, "%1.4f");
	y += 25;
	env->amplitude = GuiSlider((Rectangle){x + 20, y, 320, 20}, "Vol.", TextFormat("%1.2f", env->amplitude), env->amplitude, 0, 1);
	y += 25;
	return false;
}

float ModEnvelopeDialog::width() {
	return 400;
}

float ModEnvelopeDialog::height() {
	return 120;
}

//FilterDialog
template<typename T>
FilterDialog<T>::FilterDialog(T* filter) {
	this->filter = filter;
}

template<typename T>
bool FilterDialog<T>::draw(float x, float y) {
	scaled_slider((Rectangle){x + 60, y, 320, 20}, "Cutoff", filter->cutoff, FILTER_CUTOFF_SCALE, "%1.0f");
	y += 25;
	filter->keyboard_tracking = GuiSlider((Rectangle){x + 60, y, 320, 20}, "KB Track", TextFormat("%1.3f", filter->keyboard_tracking), filter->keyboard_tracking, 0, 1);
	y += 25;
	return false;
}

template<typename T>
float FilterDialog<T>::width() {
	return 400;
}

template<typename T>
float FilterDialog<T>::height() {
	return 50;
}

void __filter_dialog_link_fix_dont_call__ () {
	FilterDialog<LowPassFilter12Component> f(nullptr);
}

//LFODialog
LFODialog::LFODialog(LFOComponent* lfo) {
	this->lfo = lfo;
}

bool LFODialog::draw(float x, float y) {
	scaled_slider((Rectangle){x + 60, y, 320, 20}, "Freq", lfo->freq, LFO_FREQ_SCALE, "%1.3f");
	y += 25;
	lfo->amplitude = GuiSlider((Rectangle){x + 60, y, 320, 20}, "Amp", TextFormat("%1.2f", lfo->amplitude), lfo->amplitude, 0, 1);
	y += 25;
	//Modulator
	lfo->modulator = GuiCheckBox((Rectangle){x, y, 20, 20}, "Modulator", lfo->modulator);
	y += 25;
	return false;
}
float LFODialog::width() {
	return 400;
}
float LFODialog::height() {
	return 75;
}

//ControlChangeDialog
ControlChangeDialog::ControlChangeDialog(ControlChangeComponent* cc) {
	this->cc = cc;
}
bool ControlChangeDialog::draw(float x, float y) {
	int control = cc->control;
	if (GuiSpinner((Rectangle){x + 60, y, 320, 20}, "Control", &control, 0, 127, cc_editmode)) {
		cc_editmode = !cc_editmode;
	}
	cc->control = control;
	y += 25;
	cc->start = GuiSlider((Rectangle){x + 60, y, 320, 20}, "Start", TextFormat("%d", cc->start), cc->start, 0, 127);
	y += 25;
	cc->end = GuiSlider((Rectangle){x + 60, y, 320, 20}, "End", TextFormat("%d", cc->end), cc->end, 0, 127);
	y += 25;
	return false;
}
float ControlChangeDialog::width() {
	return 400;
}

float ControlChangeDialog::height() {
	return 75;
}

//VelocityDialog
VelocityDialog::VelocityDialog(VelocityComponent* vel) {
	this->vel = vel;
}
bool VelocityDialog::draw(float x, float y) {
	vel->start = GuiSlider((Rectangle){x + 60, y, 320, 20}, "Start", TextFormat("%1.2f", vel->start), vel->start, 0, 1);
	y += 25;
	vel->end = GuiSlider((Rectangle){x + 60, y, 320, 20}, "End", TextFormat("%1.2f", vel->end), vel->end, 0, 1);
	y += 25;
	return false;
}
float VelocityDialog::width() {
	return 400;
}

float VelocityDialog::height() {
	return 50;
}

//ComponentSlotDialog
ComponentSlotDialog::ComponentSlotDialog(ComponentSlot* slot) {
	this->slot = slot;
}
bool ComponentSlotDialog::draw(float x, float y) {
	bool close = false;
	if (!slot->get_component()) {
		//Create component
		if (GuiButton({x, y, 400, 20}, "Oscilator")) {
			slot->set_component(new OscilatorComponent());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "Amp Envelope")) {
			slot->set_component(new AmpEnvelopeComponent());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "Mod Envelope")) {
			slot->set_component(new ModEnvelopeComponent());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "LP12 Filter")) {
			slot->set_component(new LowPassFilter12Component());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "LP24 Filter")) {
			slot->set_component(new LowPassFilter24Component());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "HP12 Filter")) {
			slot->set_component(new HighPassFilter12Component());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "HP24 Filter")) {
			slot->set_component(new HighPassFilter24Component());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "LFO")) {
			slot->set_component(new LFOComponent());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "MIDI Control")) {
			slot->set_component(new ControlChangeComponent());
			close = true;
		}
		else if (GuiButton({x, y += 20, 400, 20}, "Velocity")) {
			slot->set_component(new VelocityComponent());
			close = true;
		}
		y += 25;
	}
	else {
		if (edit_binding) {
			//Type
			std::string options = "set;add;multiply";
			std::vector<BindingType> types = {BindingType::SET, BindingType::ADD, BindingType::MUL};
			int type = std::find(types.begin(), types.end(), binding.type) - types.begin();
			DrawText("Binding Type", x, y, 12, BLACK);
			y += 15;
			type = GuiComboBox((Rectangle){x, y, 400, 20}, options.c_str(), type);
			binding.type = types.at(type);
			y += 25;
			//Property
			std::string prop_options = "";
			std::vector<std::string> props = slot->get_component()->property_names();
			for (size_t i = 0; i < props.size(); ++i) {
				if ( i != 0) {
					prop_options += ";";
				}
				prop_options += props[i];
			}
			binding.property = GuiComboBox((Rectangle){x, y, 400, 20}, prop_options.c_str(), binding.property);
			y += 25;
			//Component
			int comp = binding.component;
			GuiSpinner((Rectangle){x + 60, y, 340, 20}, "Component", &comp, 0, MAX_COMPONENTS - 1, false);
			binding.component = comp;
			y += 25;
			//From and to
			binding.from = GuiSlider((Rectangle){x + 60, y, 320, 20}, "From", TextFormat("%1.3f", binding.from), binding.from, -1, 1);
			y += 25;
			binding.to = GuiSlider((Rectangle){x + 60, y, 320, 20}, "To", TextFormat("%1.3f", binding.to), binding.to, -1, 1);
			y += 25;
			//Apply
			if (GuiButton((Rectangle){x, y, 400, 20}, "Apply")) {
				if (binding_index < slot->bindings.size()) {
					slot->bindings[binding_index] = binding;
				}
				else {
					slot->bindings.push_back(binding);
				}
				edit_binding = false;
				binding = {};
			}
			y += 25;
			//Delete
			if (GuiButton((Rectangle){x, y, 400, 20}, "Delete")) {
				if (binding_index < slot->bindings.size()) {
					slot->bindings.erase(slot->bindings.begin() + binding_index);
				}
				edit_binding = false;
				binding = {};
			}
			y += 25;
		}
		else {
			//Bindings
			DrawText("Bindings", x, y, 12, BLACK);
			y += 15;
			std::vector<ComponentPropertyBinding> bindings = slot->bindings;
			std::vector<std::string> props = slot->get_component()->property_names();
			for (size_t i = 0; i < bindings.size(); ++i) {
				std::string name = props.at(bindings[i].property) + " to " + std::to_string(bindings[i].component);
				if (GuiButton({x, y, 400, 20}, name.c_str())) {
					edit_binding = true;
					binding = bindings[i];
					binding_index = i;
				}
				y += 20;
			}
			y += 5;
			if (GuiButton({x, y, 400, 20}, "New Binding")) {
				edit_binding = true;
				binding = {};
				binding_index = bindings.size();
			}
			y += 25;
			if (GuiButton({x, y, 400, 20}, "Delete Component")) {
				slot->set_component(nullptr);
				close = true;
			}
			y += 25;
		}
	}
	slot->audible = GuiCheckBox({x, y, 20, 20}, "Audible", slot->audible);
	y += 20;
	return close;
}

float ComponentSlotDialog::width() {
	return 400;
}

float ComponentSlotDialog::height() {
	return 20 + (slot->get_component() ? (edit_binding ? 190 : (slot->bindings.size() * 20 + 65)) : 205);
}*/

View* create_view_for_engine(std::string name, SoundEngine*engine) {
	if (name == "B3 Organ") {
		return new B3OrganEngineMenuView(&dynamic_cast<B3Organ*>(engine)->data);	//TODO cleaner check
	}
	/*else if (name == "Synthesizer") {
		return new SynthesizerEngineMenuView(&dynamic_cast<Synthesizer*>(engine)->data);	//TODO cleaner check
	}*/
	return nullptr;
}

/*
Dialog* create_dialog_for_component(std::string name, SynthComponent* data) {
	if (name == "Oscilator") {
		return new OscilatorDialog(dynamic_cast<OscilatorComponent*>(data));	//TODO cleaner check
	}
	else if (name == "Amp Envelope") {
		return new AmpEnvelopeDialog(dynamic_cast<AmpEnvelopeComponent*>(data));	//TODO cleaner check
	}
	else if (name == "Mod Envelope") {
		return new ModEnvelopeDialog(dynamic_cast<ModEnvelopeComponent*>(data));	//TODO cleaner check
	}
	else if (name == "LP12 Filter") {
		return new FilterDialog<LowPassFilter12Component>(dynamic_cast<LowPassFilter12Component*>(data));	//TODO cleaner check
	}
	else if (name == "LP24 Filter") {
		return new FilterDialog<LowPassFilter24Component>(dynamic_cast<LowPassFilter24Component*>(data));	//TODO cleaner check
	}
	else if (name == "HP12 Filter") {
		return new FilterDialog<HighPassFilter12Component>(dynamic_cast<HighPassFilter12Component*>(data));	//TODO cleaner check
	}
	else if (name == "HP24 Filter") {
		return new FilterDialog<HighPassFilter24Component>(dynamic_cast<HighPassFilter24Component*>(data));	//TODO cleaner check
	}
	else if (name == "LFO") {
		return new LFODialog(dynamic_cast<LFOComponent*>(data));	//TODO cleaner check
	}
	else if (name == "MIDI Control") {
		return new ControlChangeDialog(dynamic_cast<ControlChangeComponent*>(data));	//TODO cleaner check
	}
	else if (name == "Velocity") {
		return new VelocityDialog(dynamic_cast<VelocityComponent*>(data));	//TODO cleaner check
	}
	return nullptr;
}*/
