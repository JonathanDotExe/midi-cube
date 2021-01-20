/*
 * AnalogSynthOscilatorView.cpp
 *
 *  Created on: Jan 20, 2021
 *      Author: jojo
 */

#include "AnalogSynthOscilatorView.h"
#include "AnalogSynthView.h"
#include "resources.h"

AnalogSynthOscilatorView::AnalogSynthOscilatorView(AnalogSynth& s, SoundEngineChannel& c, int channel_index, size_t part) : synth(s), channel(c) {
	this->channel_index = channel_index;
	this->part = part;
}

void AnalogSynthOscilatorView::property_change(PropertyChange change) {
}

Scene AnalogSynthOscilatorView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	SynthPartPropertyHolder* part = &synth.parts.at(this->part);
	holders.push_back(part);

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	int tmp_x = 10;
	int tmp_y = 10;

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.change_view(new AnalogSynthView(synth, channel, channel_index));
	});
	controls.push_back(back);

	return {controls, holders};
}

AnalogSynthOscilatorView::~AnalogSynthOscilatorView() {

}
