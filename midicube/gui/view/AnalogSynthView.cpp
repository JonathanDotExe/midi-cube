/*
 * AnalogSynthView.cpp
 *
 *  Created on: 11 Jan 2021
 *      Author: jojo
 */

#include "AnalogSynthView.h"

AnalogSynthView::AnalogSynthView(AnalogSynth& s, SoundEngineChannel& c, int channel_index) : synth(s), channel(c) {
	this->channel_index = channel_index;
}

void AnalogSynthView::property_change(PropertyChange change) {
}

Scene AnalogSynthView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	/*holders.push_back(this->engine);
	SoundEngineDevice& sound_engine = frame.cube.engine;
	//Sound engines
	for (SoundEngineBank* engine : sound_engine.get_sound_engines()) {
		engine_names.push_back(engine->get_name());
	}

	//Channels
	int rows = 2;
	int cols = SOUND_ENGINE_MIDI_CHANNELS / rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		SoundEngineChannel& channel = sound_engine.channels[i];
		holders.push_back(&channel);
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);

		controls.push_back(pane);

		//Title
		Label* title = new Label("Channel " + std::to_string(i + 1), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Active
		CheckBox* active = new CheckBox(false, "", main_font, 12, x + pane_width - 30, y + 5, 20, 20);
		active->bind(&channel, SoundEngineChannelProperty::pChannelActive);
		controls.push_back(active);
		//Engine
		Button* engine = new Button("Engine", main_font, 12, x + 5, y + 30,  pane_width - 15, 30);
		engine->rect.setFillColor(sf::Color(0, 180, 255));
		controls.push_back(engine);
		engine_buttons[i] = engine;
		engine->set_on_click([&channel, i, &frame]() {
			frame.change_view(new SoundEngineChannelView(channel, i));
		});
		//Volume
		Slider* volume = new Slider(0, 0, 1, main_font, x + (pane_width - 5)/2 - 20, y + 70, 40, 180);
		volume->bind(&channel, SoundEngineChannelProperty::pChannelVolume);
		controls.push_back(volume);
	}
		//Metronome
	CheckBox* metronome = new CheckBox(false, "Metronome", main_font, 18, 10, frame.get_height() - 45, 40, 40);
	metronome->bind(this->engine, SoundEngineProperty::pEngineMetronomeOn);
	controls.push_back(metronome);

	DragBox<int>* bpm = new DragBox<int>(120, 10, 480, main_font, 18, 200, frame.get_height() - 45, 100, 40);
	bpm->drag_mul = 0.00125;
	bpm->bind(this->engine, SoundEngineProperty::pEngineMetronomeBPM);
	controls.push_back(bpm);*/

	return {controls, holders};
}

AnalogSynthView::~AnalogSynthView() {
	// TODO Auto-generated destructor stub
}
