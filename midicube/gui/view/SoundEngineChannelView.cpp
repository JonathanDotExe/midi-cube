/*
 * SoundEngineChannelView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "SoundEngineChannelView.h"
#include "resources.h"

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

	//Title
	Label* title = new Label("Channel " + std::to_string(channel_index), main_font, 18, 5, 5);
	controls.push_back(title);


	return {controls, holders};
}


SoundEngineChannelView::~SoundEngineChannelView() {

}
