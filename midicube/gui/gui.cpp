/*
 * gui.cpp
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#include "gui.h"
#include "engine/controlx.h"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>
#include <exception>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iostream>

#define PROPERTY_BIND(type, obj, expr) [&obj](type v) {expr = v;}
#define FUNC_PROPERTY_BIND(type, obj, func) [&obj](type v) {func(v);}

std::unordered_map<DeviceType, Texture2D> device_textures;

/*
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

static void scaled_slider(Rectangle pos, std::string text, double& value, const FixedScale& scale, std::string format = "1.2f%") {
	value = scale.value(GuiSlider(pos, text.c_str(), TextFormat(format.c_str(), value), scale.progress(value), 0, 1));
}
*/

//MainMenuView
MainMenuView::MainMenuView(MidiCube* cube) {
	this->cube = cube;
}

Node* MainMenuView::init(Frame* frame) {
	VBox* parent = new VBox();
	parent->style.fill_color = DARKGRAY;

	//Title
	Label* title = new Label("MIDICube");
	title->style.font_size = 72;
	title->style.font_color = WHITE;
	title->get_layout().halignment = HorizontalAlignment::CENTER;
	parent->add_child(title);

	//Button
	Button* button = new Button("Sound Engine");
	button->get_layout().margin_top = 150;
	button->get_layout().width = 400;
	button->get_layout().halignment = HorizontalAlignment::CENTER;
	button->get_layout().padding_bottom = 10;
	button->get_layout().padding_top = 10;
	button->get_layout().padding_left = 10;
	button->get_layout().padding_right = 10;
	button->style.fill_color = GREEN;
	button->style.hover_color = DARKGREEN;
	button->style.border_color = BLANK;
	button->style.border_thickness = 0;

	MidiCube* cube = this->cube;
	button->set_on_click([frame, cube]() {
		frame->request_view(new SoundEngineMenuView(cube));
	});
	parent->add_child(button);

	return parent;
}

template <typename T>
void style_button(T t) {
	t->style.border_color = BLANK;
	t->style.border_thickness = 0;
	t->style.fill_color = YELLOW;
	t->style.hover_color = ORANGE;
	t->text_style.font_size = 10;
	t->get_layout().width = MATCH_PARENT;
}

//SoundEngineMenuView
SoundEngineMenuView::SoundEngineMenuView(MidiCube* cube) {
	this->cube = cube;
}

Node* SoundEngineMenuView::init(Frame* frame) {
	VBox* container = new VBox();
	container->style.fill_color = DARKGRAY;
	HBox* box = new HBox();
	SoundEngineDevice& e = cube->engine;

	//Channels
	for (unsigned int i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		SoundEngineChannel& channel = cube->engine.get_channel(i);
		ChannelSource& source = cube->channels.at(i);

		VBox* col = new VBox();
		col->get_layout().padding_left = 1;
		col->get_layout().padding_right = 1;
		col->get_layout().x_weight = 1;
		//Title
		{
			Label* title = new Label("Ch. " + std::to_string(i + 1));
			title->style.font_size = 20;
			title->style.font_color = YELLOW;
			title->get_layout().halignment = HorizontalAlignment::CENTER;
			col->add_child(title);
		}
		//Engine
		{
			Label* engine_text = new Label("Engine");
			engine_text->style.font_size = 10;
			engine_text->style.font_color = BLACK;
			engine_text->get_layout().margin_top = 10;

			std::vector<ssize_t> engines = {};
			for (ssize_t i = -1; i < (ssize_t) cube->engine.get_sound_engines().size(); ++i) {
				engines.push_back(i);
			}

			ComboBox<ssize_t>* engine = new ComboBox<ssize_t>(engines, [&e](ssize_t b) {
				return b < 0 ? "None" : e.get_sound_engines().at(b)->get_name();
			});
			engine->set_on_change([&channel](ssize_t index) {
				channel.set_engine(index);
			});

			style_button(engine);

			col->add_child(engine_text);
			col->add_child(engine);
		}
		//Edit
		{
			Button* edit = new Button("Edit");
			edit->get_layout().margin_top = 2;
			style_button(edit);
			edit->set_on_click([&e, i, &channel, frame, this]() {
				SoundEngine* eng = channel.get_engine(e.get_sound_engines(), i);
				if (eng) {
					ViewController* view = create_view_for_engine(e.get_sound_engines().at(channel.get_engine())->get_name(), eng, cube);
					if (view) {
						frame->request_view(view);
					}
				}
			});

			col->add_child(edit);
		}
		//Octave
		{
			Label* octave_text = new Label("Octave");
			octave_text->style.font_size = 10;
			octave_text->style.font_color = BLACK;
			octave_text->get_layout().margin_top = 10;

			Spinner* octave = new Spinner(-4, 4, 0);
			octave->get_layout().width = MATCH_PARENT;
			octave->set_on_change(PROPERTY_BIND(int, source, source.octave));

			col->add_child(octave_text);
			col->add_child(octave);
		}
		//Looper
		{
			Label* looper_text = new Label("Looper");
			looper_text->style.font_size = 10;
			looper_text->style.font_color = BLACK;
			looper_text->get_layout().margin_top = 10;

			LabeledControl<CheckBox>* looper = new LabeledControl<CheckBox>("On");
			looper->get_layout().margin_top = 2;
			looper->control->set_on_change(PROPERTY_BIND(bool, channel, channel.get_looper().preset.on));

			LabeledControl<CheckBox>* looper_play = new LabeledControl<CheckBox>("Play");
			looper_play->get_layout().margin_top = 2;
			looper_play->control->checked = true;
			looper_play->control->set_on_change(PROPERTY_BIND(bool, channel, channel.get_looper().play));

			LabeledControl<CheckBox>* looper_record = new LabeledControl<CheckBox>("Record");
			looper_record->get_layout().margin_top = 2;
			looper_record->control->checked = true;
			looper_record->control->set_on_change(PROPERTY_BIND(bool, channel, channel.get_looper().record));

			LabeledControl<Spinner>* looper_bars = new LabeledControl<Spinner>("Bars", new Spinner(1, 16, 4));
			looper_bars->get_layout().margin_top = 2;
			looper_bars->get_layout().width = MATCH_PARENT;
			looper_bars->control->get_layout().width = MATCH_PARENT;
			looper_bars->control->set_on_change(PROPERTY_BIND(int, channel, channel.get_looper().preset.bars));

			Button* looper_reset = new Button("Reset");
			looper_reset->get_layout().margin_top = 2;
			style_button(looper_reset);
			looper_reset->set_on_click([&channel]() {
				channel.get_looper().reset = true;
			});

			col->add_child(looper_text);
			col->add_child(looper);
			col->add_child(looper_play);
			col->add_child(looper_record);
			col->add_child(looper_bars);
			col->add_child(looper_reset);
		}
		//Volume
		{
			Label* vol_text = new Label("Volume");
			vol_text->style.font_size = 10;
			vol_text->style.font_color = BLACK;
			vol_text->get_layout().margin_top = 10;

			Slider* volume = new Slider(channel.volume, {0, {}, 1});
			volume->style.border_color = BLANK;
			volume->style.border_thickness = 0;
			volume->button_style.border_color = BLANK;
			volume->button_style.border_thickness = 0;
			volume->button_style.fill_color = GREEN;
			volume->button_style.hover_color = DARKGREEN;
			volume->get_layout().halignment = HorizontalAlignment::CENTER;

			Label* vol_val = new Label(std::to_string(channel.volume));
			vol_val->style.font_size = 8;
			vol_val->get_layout().width = MATCH_PARENT;
			vol_val->style.text_halignment = HorizontalAlignment::LEFT;

			volume->set_on_change([&channel, vol_val](double val) {
				channel.volume = val;
				vol_val->update_text(std::to_string(val));
			});
			col->add_child(vol_text);
			col->add_child(volume);
			col->add_child(vol_val);
		}
		//Mute/Active
		{
			LabeledControl<CheckBox>* active = new LabeledControl<CheckBox>("Active");
			active->get_layout().margin_top = 5;
			active->control->checked = true;
			active->control->set_on_change(PROPERTY_BIND(bool, channel, channel.active));

			Button* solo = new Button("Solo");
			solo->get_layout().margin_top = 2;
			style_button(solo);
			solo->set_on_click([&e, i]() {
				e.solo(i);
			});

			col->add_child(active);
			col->add_child(solo);
		}
		//Input
		{
			Label* engine_text = new Label("Input");
			engine_text->style.font_size = 10;
			engine_text->style.font_color = BLACK;
			engine_text->get_layout().margin_top = 10;

			std::vector<ssize_t> inputs = {};
			for (ssize_t i = -1; i < (ssize_t) cube->get_inputs().size(); ++i) {
				inputs.push_back(i);
			}

			ComboBox<ssize_t>* input = new ComboBox<ssize_t>(inputs, [](ssize_t b) {
				return std::to_string(b);
			});

			input->set_on_change([&source](ssize_t index) {
				source.input = index;
			});

			input->style.border_color = BLACK;
			input->style.border_thickness = 1;
			input->style.fill_color = WHITE;
			input->style.hover_color = LIGHTGRAY;
			input->text_style.font_size = 10;
			input->get_layout().width = MATCH_PARENT;

			col->add_child(engine_text);
			col->add_child(input);
		}
		//Channel
		{
			Label* channel_text = new Label("Channel");
			channel_text->style.font_size = 10;
			channel_text->style.font_color = BLACK;
			channel_text->get_layout().margin_top = 10;

			std::vector<ssize_t> inputs = {};
			for (ssize_t i = -1; i < (ssize_t) cube->get_inputs().size(); ++i) {
				inputs.push_back(i);
			}

			Spinner* ch = new Spinner(1, 16, 1);
			ch->get_layout().width = MATCH_PARENT;

			ch->set_on_change([&source](int index) {
				source.channel = index - 1;
			});

			col->add_child(channel_text);
			col->add_child(ch);
		}
		//Add
		box->add_child(col);
	}
	//Metronome
	HBox* footer = new HBox();
	footer->get_layout().height = WRAP_CONTENT;
	footer->get_layout().padding_top = 5;
	footer->get_layout().padding_bottom = 5;
	footer->get_layout().padding_left = 5;
	footer->get_layout().padding_right = 5;

	LabeledControl<Spinner>* bpm = new LabeledControl<Spinner>("BPM", new Spinner(10, 480, 120), false);
	bpm->label->style.font_color = WHITE;
	bpm->get_layout().height = MATCH_PARENT;
	bpm->control->get_layout().width = 100;
	bpm->control->set_on_change(FUNC_PROPERTY_BIND(int, e, e.metronome.set_bpm));
	footer->add_child(bpm);

	LabeledControl<CheckBox>* metronome = new LabeledControl<CheckBox>("Metronome");
	metronome->label->style.font_color = WHITE;
	metronome->control->set_on_change(PROPERTY_BIND(bool, e, e.play_metronome));
	footer->add_child(metronome);

	container->add_child(box);
	container->add_child(footer);

	return container;
}

B3OrganMenuView::B3OrganMenuView(B3OrganData* data, MidiCube* cube) {
	this->data = data;
	this->cube = cube;
}

Node* B3OrganMenuView::init(Frame* frame) {
	VBox* container = new VBox();
	container->style.fill_color = DARKBROWN;

	//Title
	Label* title = new Label("B3 Organ");
	title->style.font_size = 20;
	title->style.font_color = YELLOW;
	title->get_layout().halignment = HorizontalAlignment::CENTER;
	container->add_child(title);

	//Classic organ controls
	HBox* controls = new HBox();
	controls->get_layout().height = WRAP_CONTENT;
	//Drawbars
	HBox* drawbars = new HBox();
	std::array<std::string, ORGAN_DRAWBAR_COUNT> drawbar_names = {"16'", "5 1/3'", "8'", "4'", "2 2/3'", "2'", "1 3/5'", "1 1/3'", "1'"};
	for (size_t i = 0; i < data->preset.drawbars.size(); ++i) {
		VBox* col = new VBox();
		col->get_layout().width = WRAP_CONTENT;
		col->get_layout().height = WRAP_CONTENT;
		col->get_layout().margin_top = 5;
		col->get_layout().margin_bottom = 5;
		col->get_layout().margin_left = 5;
		col->get_layout().margin_right = 5;
		//Name
		Label* name = new Label(drawbar_names.at(i));
		name->style.font_size = 10;
		name->style.font_color = YELLOW;
		name->get_layout().halignment = HorizontalAlignment::CENTER;
		//Value
		Label* value = new Label(std::to_string(data->preset.drawbars[i]));
		value->style.font_size = 10;
		value->style.font_color = WHITE;
		value->get_layout().halignment = HorizontalAlignment::CENTER;
		//Drawbar
		OrganDrawbar* drawbar = new OrganDrawbar(data->preset.drawbars[i]);
		Color color = WHITE;
		if (i <= 1) {
			color = RED;
		}
		else if (i == 4 || i == 6 || i == 7) {
			color = BLACK;
		}
		drawbar->get_layout().height = 200;
		drawbar->get_layout().width = 30;
		drawbar->button_style.fill_color = color;
		drawbar->button_style.hover_color = color;
		drawbar->style.fill_color = LIGHTGRAY;
		drawbar->style.hover_color = LIGHTGRAY;
		drawbar->style.border_thickness = 0;
		drawbar->style.border_color = BLANK;

		B3OrganPreset& preset = data->preset;
		drawbar->set_on_change([&preset,  value, i](int val) {
			preset.drawbars[i] = val;
			value->update_text(std::to_string(val));
		});

		col->add_child(name);
		col->add_child(drawbar);
		col->add_child(value);

		drawbars->add_child(col);
	}
	container->add_child(drawbars);

	return container;
}


ViewController* create_view_for_engine(std::string name, SoundEngine*engine, MidiCube* cube) {
	if (name == "B3 Organ") {
		return new B3OrganMenuView(&dynamic_cast<B3Organ*>(engine)->data, cube);	//TODO cleaner check
	}
	/*else if (name == "Synthesizer") {
		return new SynthesizerEngineMenuView(&dynamic_cast<Synthesizer*>(engine)->data);	//TODO cleaner check
	}*/
	return nullptr;
}

/*
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
}

View* create_view_for_engine(std::string name, SoundEngine*engine) {
	if (name == "B3 Organ") {
		return new B3OrganEngineMenuView(&dynamic_cast<B3Organ*>(engine)->data);	//TODO cleaner check
	}
	else if (name == "Synthesizer") {
		return new SynthesizerEngineMenuView(&dynamic_cast<Synthesizer*>(engine)->data);	//TODO cleaner check
	}
	return nullptr;
}

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
