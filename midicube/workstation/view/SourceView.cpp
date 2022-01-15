/*
 * SourceView.cpp
 *
 *  Created on: Jun 18, 2021
 *      Author: jojo
 */

#include "../view/SourceView.h"

#include "../view/SoundEngineView.h"

SourceView::SourceView(MidiCubeWorkstation& c) : cube(c) {

}

Scene SourceView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.get_action_handler();

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Channels
	int rows = 2;
	int cols = SOUND_ENGINE_MIDI_CHANNELS / rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		MidiSource& source = cube.sources[i];
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);
		controls.push_back(pane);

		//Title
		Label* title = new Label("Source " + std::to_string(i), main_font, 16, x + 5, y + 5);
		controls.push_back(title);
		y += 30;

		//Device
		{
			DragBox<int>* value = new DragBox<int>(0, -1, SOUND_ENGINE_MIDI_CHANNELS - 1, main_font, 12, x + 5, y, pane_width/2 - 5, 30);
			value->property.bind(source.device, handler);
			controls.push_back(value);

			Label* label = new Label("Device", main_font, 12, x + 5 + pane_width/2, y + 5);
			controls.push_back(label);
		}
		y += 40;
		//Channel
		{
			DragBox<int>* value = new DragBox<int>(0, -1, SOUND_ENGINE_MIDI_CHANNELS - 1, main_font, 12, x + 5, y, pane_width/2 - 5, 30);
			value->property.bind(source.channel, handler);
			controls.push_back(value);

			Label* label = new Label("Channel", main_font, 12, x + 5 + pane_width/2, y + 5);
			controls.push_back(label);
		}
		y += 40;
	}

	//Control Source
	{
		DragBox<int>* value = new DragBox<int>(0, 0, SOUND_ENGINE_MIDI_CHANNELS, main_font, 18, 5, frame.get_height() - 45, 60, 40);
		value->property.bind(cube.control_source, handler);
		controls.push_back(value);

		Label* label = new Label("Control Source", main_font, 18, 70, frame.get_height() - 37);
		controls.push_back(label);
	}

	//Clock Source
	{
		DragBox<int>* value = new DragBox<int>(0, 0, SOUND_ENGINE_MIDI_CHANNELS, main_font, 18, 200, frame.get_height() - 45, 60, 40);
		value->property.bind(cube.clock_source, handler);
		controls.push_back(value);

		Label* label = new Label("Clock Source", main_font, 18, 265, frame.get_height() - 37);
		controls.push_back(label);
	}

	//Exit Button
	Button* exit = new Button("Back", main_font, 18, frame.get_width() - 75, frame.get_height() - 45, 70, 40);
	exit->set_on_click([&frame, this]() {
		frame.menu_back();
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}

void SourceView::update_properties() {
}

SourceView::~SourceView() {

}
