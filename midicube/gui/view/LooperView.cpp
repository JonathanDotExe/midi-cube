/*
 * LooperView.cpp
 *
 *  Created on: Apr 14, 2021
 *      Author: jojo
 */

#include "LooperView.h"
#include "resources.h"
#include "SoundEngineView.h"

LooperView::LooperView() {

}

LooperView::~LooperView() {

}

Scene LooperView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.cube.action_handler;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	SoundEngineDevice& sound_engine = frame.cube.engine;

	//Channels
	int rows = 2;
	int cols = SOUND_ENGINE_MIDI_CHANNELS / rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		SoundEngineChannel& channel = sound_engine.channels[i];
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
		active->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_active, &channel), std::bind(&SoundEngineChannel::set_active, &channel, std::placeholders::_1), handler);
		controls.push_back(active);

		//Volume
		Slider* volume = new Slider(0, 0, 1, main_font, x + (pane_width - 5)/2 - 20, y + 70, 40, 180);
		volume->property.bind(channel.volume, handler);
		controls.push_back(volume);
	}

	//Metronome
	CheckBox* metronome = new CheckBox(false, "Metronome", main_font, 18, 10, frame.get_height() - 45, 40, 40);
	metronome->property.bind(sound_engine.play_metronome, handler);
	controls.push_back(metronome);

	DragBox<int>* bpm = new DragBox<int>(120, 10, 480, main_font, 18, 200, frame.get_height() - 45, 100, 40);
	bpm->drag_mul = 0.00125;
	bpm->property.bind_function<unsigned int>(std::bind(&Metronome::get_bpm, &sound_engine.metronome), std::bind(&Metronome::set_bpm, &sound_engine.metronome, std::placeholders::_1), handler);
	controls.push_back(bpm);

	//Volume
	DragBox<double>* volume = new DragBox<double>(0, 0, 1, main_font, 18, 330, frame.get_height() - 45, 100, 40);
	volume->property.bind(sound_engine.volume, handler);
	controls.push_back(volume);

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}
