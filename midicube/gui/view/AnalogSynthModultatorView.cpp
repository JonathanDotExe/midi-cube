/*
 * AnalogSynthModultatorView.cpp
 *
 *  Created on: Jan 24, 2021
 *      Author: jojo
 */

#include "AnalogSynthModultatorView.h"
#include "AnalogSynthView.h"
#include "resources.h"

AnalogSynthModultatorView::AnalogSynthModultatorView(AnalogSynth &s,
		SoundEngineChannel &c, int channel_index, size_t part) : synth(s), channel(c) {
	this->channel_index = channel_index;
	this->part = part;
}

void AnalogSynthModultatorView::property_change(PropertyChange change) {
}

Scene AnalogSynthModultatorView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<Control*> show_amount;
	std::vector<Control*> show_source;
	std::vector<PropertyHolder*> holders;
	std::cout << part << std::endl;

	SynthPartPropertyHolder* part = &synth.parts.at(this->part);
	holders.push_back(part);

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	int tmp_x = 10;
	int tmp_y = 10;



	//Edit Sources
	Button* edit = new Button("Edit Sources", main_font, 18, frame.get_width() - 70 - 120, frame.get_height() - 40, 120, 40);
	edit->rect.setFillColor(sf::Color::Yellow);
	edit->set_on_click([&frame, show_amount, show_source, this]() {
		edit_source = !edit_source;
		for (Control* c : show_amount) {
			c->set_visible(!edit_source);
		}
		for (Control* c : show_source) {
			c->set_visible(edit_source);
		}
	});
	controls.push_back(edit);

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.change_view(new AnalogSynthView(synth, channel, channel_index));
	});
	controls.push_back(back);

	for (Control* c : show_amount) {
		c->set_visible(!edit_source);
	}
	for (Control* c : show_source) {
		c->set_visible(edit_source);
	}

	return {controls, holders};
}

AnalogSynthModultatorView::~AnalogSynthModultatorView() {

}
