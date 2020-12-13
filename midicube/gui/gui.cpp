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
			engine->set_value(channel.get_engine());
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

			Spinner* octave = new Spinner(-4, 4, source.octave);
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
			looper->control->checked = channel.get_looper().preset.on;
			looper->get_layout().margin_top = 2;
			looper->control->set_on_change(PROPERTY_BIND(bool, channel, channel.get_looper().preset.on));

			LabeledControl<CheckBox>* looper_play = new LabeledControl<CheckBox>("Play");
			looper_play->get_layout().margin_top = 2;
			looper_play->control->checked = channel.get_looper().play;
			looper_play->control->set_on_change(PROPERTY_BIND(bool, channel, channel.get_looper().play));

			LabeledControl<CheckBox>* looper_record = new LabeledControl<CheckBox>("Record");
			looper_record->get_layout().margin_top = 2;
			looper_record->control->checked = channel.get_looper().record;
			looper_record->control->set_on_change(PROPERTY_BIND(bool, channel, channel.get_looper().record));

			LabeledControl<Spinner>* looper_bars = new LabeledControl<Spinner>("Bars", new Spinner(1, 16, channel.get_looper().preset.bars));
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
			volume->set_value(channel.volume);
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
			active->control->checked = channel.active;
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
			input->set_value(source.input);

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

			Spinner* ch = new Spinner(1, 16, source.channel + 1);
			ch->get_layout().width = MATCH_PARENT;

			ch->set_on_change([&source](int index) {
				source.channel = index - 1;
			});

			col->add_child(channel_text);
			col->add_child(ch);
		}
		//Start note
		{
			Label* start_note_text = new Label("Start Note");
			start_note_text->style.font_size = 10;
			start_note_text->style.font_color = BLACK;
			start_note_text->get_layout().margin_top = 10;

			Spinner* ch = new Spinner(0, 127, source.start_note);
			ch->get_layout().width = MATCH_PARENT;

			ch->set_on_change([&source](int note) {
				source.start_note = note;
			});

			col->add_child(start_note_text);
			col->add_child(ch);
		}
		//End note
		{
			Label* end_note_text = new Label("End Note");
			end_note_text->style.font_size = 10;
			end_note_text->style.font_color = BLACK;
			end_note_text->get_layout().margin_top = 10;

			Spinner* ch = new Spinner(0, 127, source.end_note);
			ch->get_layout().width = MATCH_PARENT;

			ch->set_on_change([&source](int note) {
				source.end_note = note;
			});

			col->add_child(end_note_text);
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

	LabeledControl<Spinner>* bpm = new LabeledControl<Spinner>("BPM", new Spinner(10, 480, e.metronome.get_bpm()), false);
	bpm->label->style.font_color = WHITE;
	bpm->get_layout().height = MATCH_PARENT;
	bpm->control->get_layout().width = 100;
	bpm->control->set_on_change(FUNC_PROPERTY_BIND(int, e, e.metronome.set_bpm));
	footer->add_child(bpm);

	LabeledControl<CheckBox>* metronome = new LabeledControl<CheckBox>("Metronome");
	metronome->control->checked = e.play_metronome;
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

static void style_organ_switch(LabeledControl<OrganSwitch>* sw) {
	sw->label->style.font_color = WHITE;
	sw->control->get_layout().width = 25;
	sw->control->get_layout().height = 30;
	sw->get_layout().margin_bottom = 5;
}

static void style_organ_drag_box(LabeledControl<DragBox>* sw) {
	sw->label->style.font_color = WHITE;
	sw->control->step = 0.005;
	sw->control->get_layout().width = 25;
	sw->control->get_layout().height = 30;
	sw->get_layout().margin_bottom = 5;
}

static void style_organ_midi_cc(LabeledControl<Spinner>* sw) {
	sw->label->style.font_color = WHITE;
	sw->control->get_layout().width = 35;
	sw->control->get_layout().height = 20;
	sw->get_layout().margin_bottom = 5;
}

Node* B3OrganMenuView::init(Frame* frame) {
	VBox* container = new VBox();
	container->style.fill_color = DARKBROWN;

	B3OrganPreset& preset = data->preset;

	std::vector<Node*> show_midi;
	std::vector<Node*> hide_midi;

	//Title
	Label* title = new Label("B3 Organ");
	title->style.font_size = 20;
	title->style.font_color = YELLOW;
	title->get_layout().halignment = HorizontalAlignment::CENTER;
	container->add_child(title);

	//Classic organ controls
	HBox* controls = new HBox();
	controls->get_layout().height = WRAP_CONTENT;
	//Switches
	VBox* switches = new VBox();
	{
		//Rotary
		LabeledControl<OrganSwitch>* rotary_switch = new LabeledControl<OrganSwitch>("Rotary", new OrganSwitch(preset.rotary));
		style_organ_switch(rotary_switch);
		rotary_switch->control->set_on_change(PROPERTY_BIND(bool, preset, preset.rotary));
		switches->add_child(rotary_switch);
		hide_midi.push_back(rotary_switch);
		//Rotary Speed
		LabeledControl<OrganSwitch>* rotary_speed_switch = new LabeledControl<OrganSwitch>("Rotary Speed", new OrganSwitch(data->preset.rotary_fast, "FST", "SLW"));
		style_organ_switch(rotary_speed_switch);
		rotary_speed_switch->control->set_on_change(PROPERTY_BIND(bool, preset, preset.rotary_fast));
		switches->add_child(rotary_speed_switch);
		hide_midi.push_back(rotary_speed_switch);

		//Percussion
		LabeledControl<OrganSwitch>* percussion_switch = new LabeledControl<OrganSwitch>("Percussion", new OrganSwitch(data->preset.percussion));
		style_organ_switch(percussion_switch);
		percussion_switch->control->set_on_change(PROPERTY_BIND(bool, preset, preset.percussion));
		switches->add_child(percussion_switch);
		hide_midi.push_back(percussion_switch);

		//Percussion Harmonic
		LabeledControl<OrganSwitch>* percussion_harmonic_switch = new LabeledControl<OrganSwitch>("Percussion Harmonic", new OrganSwitch(data->preset.percussion_third_harmonic, "3rd", "2nd"));
		style_organ_switch(percussion_harmonic_switch);
		percussion_harmonic_switch->control->set_on_change(PROPERTY_BIND(bool, preset, preset.percussion_third_harmonic));
		switches->add_child(percussion_harmonic_switch);
		hide_midi.push_back(percussion_harmonic_switch);

		//Percussion Volume
		LabeledControl<OrganSwitch>* percussion_volume_switch = new LabeledControl<OrganSwitch>("Percussion Volume", new OrganSwitch(data->preset.percussion_soft, "SFT", "HRD"));
		style_organ_switch(percussion_volume_switch);
		percussion_volume_switch->control->set_on_change(PROPERTY_BIND(bool, preset, preset.percussion_soft));
		switches->add_child(percussion_volume_switch);
		hide_midi.push_back(percussion_volume_switch);

		//Percussion Decay
		LabeledControl<OrganSwitch>* percussion_decay_switch = new LabeledControl<OrganSwitch>("Percussion Decay", new OrganSwitch(data->preset.percussion_fast_decay, "FST", "SLW"));
		style_organ_switch(percussion_decay_switch);
		percussion_decay_switch->control->set_on_change(PROPERTY_BIND(bool, preset, preset.percussion_fast_decay));
		switches->add_child(percussion_decay_switch);
		hide_midi.push_back(percussion_decay_switch);
	}

	{
		//Rotary
		LabeledControl<Spinner>* rotary_switch = new LabeledControl<Spinner>("Rotary", new Spinner(0, 127, preset.rotary_cc));
		style_organ_midi_cc(rotary_switch);
		rotary_switch->control->set_on_change(PROPERTY_BIND(int, preset, preset.rotary_cc));
		switches->add_child(rotary_switch);
		show_midi.push_back(rotary_switch);

		//Rotary Speed
		LabeledControl<Spinner>* rotary_speed_switch = new LabeledControl<Spinner>("Rotary Speed", new Spinner(0, 127, data->preset.rotary_speed_cc));
		style_organ_midi_cc(rotary_speed_switch);
		rotary_speed_switch->control->set_on_change(PROPERTY_BIND(int, preset, preset.rotary_speed_cc));
		switches->add_child(rotary_speed_switch);
		show_midi.push_back(rotary_speed_switch);

		//Percussion
		LabeledControl<Spinner>* percussion_switch = new LabeledControl<Spinner>("Percussion", new Spinner(0, 127, data->preset.percussion_cc));
		style_organ_midi_cc(percussion_switch);
		percussion_switch->control->set_on_change(PROPERTY_BIND(int, preset, preset.percussion_cc));
		switches->add_child(percussion_switch);
		show_midi.push_back(percussion_switch);

		//Percussion Harmonic
		LabeledControl<Spinner>* percussion_harmonic_switch = new LabeledControl<Spinner>("Percussion Harmonic", new Spinner(0, 127, data->preset.percussion_third_harmonic_cc));
		style_organ_midi_cc(percussion_harmonic_switch);
		percussion_harmonic_switch->control->set_on_change(PROPERTY_BIND(int, preset, preset.percussion_third_harmonic_cc));
		switches->add_child(percussion_harmonic_switch);
		show_midi.push_back(percussion_harmonic_switch);

		//Percussion Volume
		LabeledControl<Spinner>* percussion_volume_switch = new LabeledControl<Spinner>("Percussion Volume", new Spinner(0, 127, data->preset.percussion_soft_cc));
		style_organ_midi_cc(percussion_volume_switch);
		percussion_volume_switch->control->set_on_change(PROPERTY_BIND(int, preset, preset.percussion_soft_cc));
		switches->add_child(percussion_volume_switch);
		show_midi.push_back(percussion_volume_switch);

		//Percussion Decay
		LabeledControl<Spinner>* percussion_decay_switch = new LabeledControl<Spinner>("Percussion Decay", new Spinner(0, 127, data->preset.percussion_fast_decay_cc));
		style_organ_midi_cc(percussion_decay_switch);
		percussion_decay_switch->control->set_on_change(PROPERTY_BIND(int, preset, preset.percussion_fast_decay_cc));
		switches->add_child(percussion_decay_switch);
		show_midi.push_back(percussion_decay_switch);
	}

	controls->add_child(switches);
	//Drawbars
	HBox* drawbars = new HBox();
	drawbars->get_layout().x_weight = 2;
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
		hide_midi.push_back(value);
		//MIDI
		std::atomic<unsigned int>& dr_cc = data->preset.drawbar_ccs[i];
		Spinner* drawbar_cc = new Spinner(0, 127, dr_cc);
		drawbar_cc->set_on_change(PROPERTY_BIND(int, dr_cc, dr_cc));
		show_midi.push_back(drawbar_cc);
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
		col->add_child(drawbar_cc);

		drawbars->add_child(col);
	}
	controls->add_child(drawbars);

	//Amplifier
	HBox* amplifier = new HBox();
	VBox* distortion_box = new VBox();
	distortion_box->get_layout().width = WRAP_CONTENT;
	distortion_box->get_layout().height = WRAP_CONTENT;

	//Overdrive
	Label* vol_text = new Label("Overdrive");
	vol_text->style.font_size = 10;
	vol_text->style.font_color = BLACK;
	vol_text->get_layout().halignment = HorizontalAlignment::CENTER;
	vol_text->get_layout().margin_top = 10;

	Slider* overdrive = new Slider(preset.overdrive, {0, {}, 1});
	overdrive->style.border_color = BLANK;
	overdrive->style.border_thickness = 0;
	overdrive->button_style.border_color = BLANK;
	overdrive->button_style.border_thickness = 0;
	overdrive->button_style.fill_color = BLACK;
	overdrive->button_style.hover_color = DARKGRAY;
	overdrive->get_layout().halignment = HorizontalAlignment::CENTER;

	Label* overdrive_val = new Label(std::to_string(preset.overdrive));
	overdrive_val->style.font_size = 8;
	overdrive_val->get_layout().width = MATCH_PARENT;
	overdrive_val->get_layout().halignment = HorizontalAlignment::CENTER;
	hide_midi.push_back(overdrive_val);

	Spinner* overdrive_cc = new Spinner(0, 127, preset.overdrive_cc);
	overdrive_cc->set_on_change(PROPERTY_BIND(int, preset, preset.overdrive_cc));
	show_midi.push_back(overdrive_cc);

	overdrive->set_on_change([&preset, overdrive_val](double val) {
		preset.overdrive = val;
		overdrive_val->update_text(std::to_string(val));
	});

	distortion_box->add_child(vol_text);
	distortion_box->add_child(overdrive);
	distortion_box->add_child(overdrive_val);
	distortion_box->add_child(overdrive_cc);
	//DistortionType
	std::vector<ssize_t> types{0, 1, 2};
	std::vector<DistortionType> values{DistortionType::DIGITAL, DistortionType::ANALOG_1, DistortionType::ANALOG_2};
	std::vector<std::string> names{"Digital", "Analog 1", "Analog 2"};

	ComboBox<ssize_t>* distortion_type = new ComboBox<ssize_t>(types, [names](ssize_t b) {
		return names.at(b);
	});
	distortion_type->get_layout().margin_left = 10;
	distortion_type->get_layout().width = 60;
	distortion_type->set_on_change([&preset, values](ssize_t index) {
		preset.distortion_type = values.at(index);
	});

	amplifier->add_child(distortion_box);
	amplifier->add_child(distortion_type);
	controls->add_child(amplifier);

	container->add_child(controls);

	//Physical modeling controls
	HBox* pm_params = new HBox();
	pm_params->get_layout().height = WRAP_CONTENT;
	VBox* col1 = new VBox();
	//Harmonic foldback volume
	LabeledControl<DragBox>* foldback_vol = new LabeledControl<DragBox>("Harmonic Foldback Volume", new DragBox(preset.harmonic_foldback_volume, {0, {}, 1}));
	style_organ_drag_box(foldback_vol);
	foldback_vol->control->set_on_change(PROPERTY_BIND(double, preset, preset.harmonic_foldback_volume));
	col1->add_child(foldback_vol);

	//Rotary gain
	LabeledControl<DragBox>* rotary_gain = new LabeledControl<DragBox>("Rotary Gain", new DragBox(preset.rotary_gain, {0, {}, 2}));
	style_organ_drag_box(rotary_gain);
	rotary_gain->control->set_on_change(PROPERTY_BIND(double, preset, preset.rotary_gain));
	col1->add_child(rotary_gain);

	//Rotary type
	LabeledControl<OrganSwitch>* rotary_type = new LabeledControl<OrganSwitch>("Rotary Type", new OrganSwitch(preset.rotary_type, "1", "2"));
	style_organ_switch(rotary_type);
	rotary_type->label->style.font_color = WHITE;
	rotary_type->control->set_on_change(PROPERTY_BIND(double, preset, preset.rotary_type));
	col1->add_child(rotary_type);

	//Rotary stereo mix
	LabeledControl<DragBox>* rotary_stereo_mix = new LabeledControl<DragBox>("Rotary Stereo Mix", new DragBox(preset.rotary_stereo_mix, {0, {}, 1}));
	style_organ_drag_box(rotary_stereo_mix);
	rotary_stereo_mix->control->set_on_change(PROPERTY_BIND(double, preset, preset.rotary_stereo_mix));
	col1->add_child(rotary_stereo_mix);

	pm_params->add_child(col1);
	container->add_child(pm_params);

	//Footer
	HBox* footer = new HBox();
	footer->get_layout().margin_top = 10;
	footer->get_layout().height = WRAP_CONTENT;
	//Home
	Button* home_button = new Button("Home");
	home_button->set_on_click([this, frame]() {
		frame->request_view(new SoundEngineMenuView(cube));
	});
	footer->add_child(home_button);

	//Edit MIDI
	LabeledControl<CheckBox>* edit_midi = new LabeledControl<CheckBox>("Edit MIDI");
	edit_midi->label->style.font_color = WHITE;
	edit_midi->control->set_on_change([show_midi, hide_midi](bool midi) {
		for (Node* node : show_midi) {
			node->set_visible(midi);
		}
		for (Node* node : hide_midi) {
			node->set_visible(!midi);
		}
	});

	footer->add_child(edit_midi);
	container->add_child(footer);

	//Hide
	for (Node* node : show_midi) {
		node->set_visible(false);
	}
	//Show
	for (Node* node : hide_midi) {
		node->set_visible(true);
	}

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

