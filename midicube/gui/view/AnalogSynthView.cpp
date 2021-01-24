/*
 * AnalogSynthView.cpp
 *
 *  Created on: 11 Jan 2021
 *      Author: jojo
 */

#include "AnalogSynthView.h"
#include "resources.h"
#include "SoundEngineChannelView.h"
#include "AnalogSynthOscilatorView.h"
#include "AnalogSynthModulatorView.h"

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

	//Channels
	int cols = synth.parts.size();
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / 2;
	for (size_t i = 0; i < synth.parts.size(); ++i) {
		SynthPartPropertyHolder& part = synth.parts[i];
		holders.push_back(&part);
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10;
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);

		controls.push_back(pane);

		//Title
		Label* title = new Label("Part " + std::to_string(i + 1), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Oscilator
		Button* osc = new Button("Oscilator", main_font, 14, x + 5, y + 30,  pane_width - 15, 30);
		osc->rect.setFillColor(sf::Color(0, 180, 255));
		osc->set_on_click([&frame, this, i]{
			frame.change_view(new AnalogSynthOscilatorView(synth, channel, channel_index, i));
		});
		controls.push_back(osc);
		//Modulators
		Button* mod = new Button("Modulation", main_font, 14, x + 5, y + 70,  pane_width - 15, 30);
		mod->rect.setFillColor(sf::Color(0, 180, 255));
		mod->set_on_click([&frame, this, i]{
			frame.change_view(new AnalogSynthModulatorView(synth, channel, channel_index, i));
		});
		controls.push_back(mod);
	}

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineChannelView(channel, channel_index));
	});
	controls.push_back(back);

	return {controls, holders};
}

AnalogSynthView::~AnalogSynthView() {
	// TODO Auto-generated destructor stub
}
