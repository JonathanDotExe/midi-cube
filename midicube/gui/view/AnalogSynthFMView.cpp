/*
 * AnalogSynthFMView.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: jojo
 */

#include "AnalogSynthFMView.h"
#include "AnalogSynthView.h"

AnalogSynthFMView::AnalogSynthFMView(AnalogSynth& s, SoundEngineChannel& c, int channel_index) : synth(s), channel(c) {
	this->channel_index = channel_index;
}

void AnalogSynthFMView::property_change(PropertyChange change) {
}

Scene AnalogSynthFMView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;
	holders.push_back(&synth);

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	int tmp_x = 10;
	int tmp_y = 10;

	//Title
	Label* carrier = new Label("Carrier", main_font, 18, tmp_x + 85 * ANALOG_PART_COUNT/2 - 50, tmp_y);
	controls.push_back(carrier);
	tmp_y += 25;

	for (int i = 0; i < ANALOG_PART_COUNT; ++i) {
		Label* osc = new Label("Osc " + std::to_string(i + 1), main_font, 12, tmp_x + i * 45, tmp_y);
		controls.push_back(osc);
	}
	tmp_y += 15;

	//Modulator
	Label* modulator = new Label("Modulator", main_font, 18, tmp_x, tmp_y + 85 * ANALOG_PART_COUNT/2 - 10);
	controls.push_back(modulator);

	//Grid
	for (size_t i = 0; i < ANALOG_PART_COUNT; ++i) {
		SynthPartPropertyHolder& part = synth.parts[i];
		holders.push_back(&part);

		int y = tmp_y + i * 45;
		Label* osc = new Label("Osc " + std::to_string(i + 1), main_font, 12, tmp_x, y + 10);
		controls.push_back(osc);
		for (size_t j = 0; j < ANALOG_PART_COUNT; ++j) {
			int x = tmp_x + j * 85 + 50;
			DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y, 80, 40);
			value->bind(&part, SynthPartProperty::pSynthOscFM, j);
			//Color if feedback
			if (i >= j) {
				value->rect.setFillColor(sf::Color(180, 180, 180));
			}
			controls.push_back(value);
		}
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

AnalogSynthFMView::~AnalogSynthFMView() {
	// TODO Auto-generated destructor stub
}
