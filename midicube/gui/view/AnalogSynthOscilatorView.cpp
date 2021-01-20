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

	//Col 1
	//Waveform and active
	{
		std::vector<std::string> waveforms = {"Sine", "Saw Down", "Saw Up", "Square", "Noise"};

		ComboBox* waveform = new ComboBox(1, waveforms, main_font, 16, 0, tmp_x , tmp_y, 100, 40);
		waveform->bind(part, SynthPartProperty::pSynthOscWaveForm);
		controls.push_back(waveform);
	}
	{
		CheckBox* active = new CheckBox(false, "Active", main_font, 16, tmp_x + 110, tmp_y, 40, 40);
		active->bind(part, SynthPartProperty::pSynthOscActive);
		controls.push_back(active);
		tmp_y += 45;
	}

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
