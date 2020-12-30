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
	Label* title = new Label("Channel " + std::to_string(channel_index + 1), main_font, 18, 10, 10);
	controls.push_back(title);

	//Engine
	ComboBox* engine = new ComboBox(0, engine_names, main_font, 12, -1, 10, 40, 200, 40);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->bind(&channel, SoundEngineChannelProperty::pChannelSoundEngine);
	controls.push_back(engine);
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
