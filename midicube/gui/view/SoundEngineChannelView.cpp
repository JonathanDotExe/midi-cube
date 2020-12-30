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

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	//Title
	Label* title = new Label("Channel " + std::to_string(channel_index + 1), main_font, 18, 5, 5);
	controls.push_back(title);

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
