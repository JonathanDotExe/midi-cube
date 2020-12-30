/*
 * SoundEngineChannelView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "SoundEngineChannelView.h"
#include "resources.h"
#include "SoundEngineView.h"

SoundEngineChannelView::SoundEngineChannelView(SoundEngineChannel& ch, int channel_index) : channel(ch) {
	this->channel_index = channel_index;
}

void SoundEngineChannelView::property_change(PropertyChange change) {
}

Scene SoundEngineChannelView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	holders.push_back(&channel);

	//Sound engines
	std::vector<std::string> engine_names;
	engine_names.push_back("None");
	for (SoundEngineBank* engine : frame.cube.engine.get_sound_engines()) {
		engine_names.push_back(engine->get_name());
	}

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	//Title
	Label* title = new Label("Channel " + std::to_string(channel_index + 1), main_font, 24, 10, 10);
	controls.push_back(title);

	//Col 1
	//Engine
	ComboBox* engine = new ComboBox(0, engine_names, main_font, 18, -1, 10, 45, 200, 40);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->bind(&channel, SoundEngineChannelProperty::pChannelSoundEngine);
	controls.push_back(engine);

	//Col 2
	//Volume
	{
		Label* vol_label = new Label("Vol.", main_font, 24, 230, 10);
		controls.push_back(vol_label);

		Slider* volume = new Slider(0, 0, 1, main_font, 230, 45, 40, 300, 0.7, 0.1);
		volume->bind(&channel, SoundEngineChannelProperty::pChannelVolume);
		controls.push_back(volume);
	}
	//Panning
	{
		Label* pan_label = new Label("Pan.", main_font, 24, 290, 10);
		controls.push_back(pan_label);

		Slider* pan = new Slider(0, 0, 1, main_font, 290, 45, 40, 300, 0.7, 0.1);
		//pan->bind(&channel, SoundEngineChannelProperty::pChannelVolume); TODO implement channel pan
		controls.push_back(pan);
	}

	//Col 3
	//Source
	{
		Label* source_label = new Label("Source", main_font, 18, 350, 15);
		controls.push_back(source_label);

		DragBox<int>* source = new DragBox<int>(0, -1, 15, main_font, 18, 350, 45, 100, 40);
		source->bind(&channel, SoundEngineChannelProperty::pChannelInputDevice);
		controls.push_back(source);
	}
	//Channel
	{
		Label* channel_label = new Label("Channel", main_font, 18, 350, 90);
		controls.push_back(channel_label);

		DragBox<int>* input_channel = new DragBox<int>(0, 0, 15, main_font, 18, 350, 120, 100, 40);
		input_channel->bind(&channel, SoundEngineChannelProperty::pChannelInputChannel);
		controls.push_back(input_channel);
	}
	//Octave
	{
		Label* octave_label = new Label("Octave", main_font, 18, 350, 165);
		controls.push_back(octave_label);

		DragBox<int>* octave = new DragBox<int>(0, -3, 3, main_font, 18, 350, 195, 100, 40);
		octave->bind(&channel, SoundEngineChannelProperty::pChannelOctave);
		controls.push_back(octave);
	}

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	controls.push_back(back);

	return {controls, holders};
}


SoundEngineChannelView::~SoundEngineChannelView() {

}
