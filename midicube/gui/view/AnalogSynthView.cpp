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
#include "AnalogSynthFMView.h"

AnalogSynthView::AnalogSynthView(AnalogSynth& s, SoundEngineChannel& c, int channel_index) : synth(s), channel(c) {
	this->channel_index = channel_index;
}

void AnalogSynthView::property_change(PropertyChange change) {
}

Scene AnalogSynthView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;
	holders.push_back(&synth);

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Channels
	int cols = synth.parts.size();
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = 150;
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

	//Part counts
	int tmp_x = 10;
	int tmp_y = pane_height + 20;
	//Oscilators
	{
		Label* title = new Label("Osc Count", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<int>* value = new DragBox<int>(0, 0, ANALOG_PART_COUNT, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->bind(&synth, SynthProperty::pSynthOpCount);
		controls.push_back(value);
	}
	tmp_x += 90;
	//Mod Env Count
	{
		Label* title = new Label("Env Count", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<int>* value = new DragBox<int>(0, 0, ANALOG_PART_COUNT, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->bind(&synth, SynthProperty::pSynthModEnvCount);
		controls.push_back(value);
	}
	tmp_x += 90;
	{
		Label* title = new Label("LFO Count", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);
		DragBox<int>* value = new DragBox<int>(0, 0, ANALOG_PART_COUNT, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->bind(&synth, SynthProperty::pSynthLFOCount);
		controls.push_back(value);
	}
	tmp_y += 75;
	tmp_x = 10;

	//Polyphony Mode
	//Mono
	{
		CheckBox* mono = new CheckBox(false, "Mono", main_font, 16, tmp_x, tmp_y + 15, 40, 40);
		mono->bind(&synth, SynthProperty::pSynthMono);
		controls.push_back(mono);
	}
	tmp_x += 120;
	//Legato
	{
		CheckBox* legato = new CheckBox(false, "Legato", main_font, 16, tmp_x, tmp_y + 15, 40, 40);
		legato->bind(&synth, SynthProperty::pSynthLegato);
		controls.push_back(legato);
	}
	tmp_x += 120;
	//Portamendo
	{
		Label* title = new Label("Portamendo", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 5, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul /= 5;
		value->bind(&synth, SynthProperty::pSynthPortamendo);
		controls.push_back(value);
	}
	tmp_x = 10;
	tmp_y += 75;

	//Delay
	//Delay Mix
	{
		Label* title = new Label("Delay Mix", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->bind(&synth, SynthProperty::pSynthDelayMix);
		controls.push_back(value);
	}
	tmp_x += 90;
	//Delay Time
	{
		Label* title = new Label("Delay Time", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 5, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->bind(&synth, SynthProperty::pSynthDelayTime);
		controls.push_back(value);
	}
	tmp_x += 90;
	//Delay Feedback
	{
		Label* title = new Label("Delay Feedback", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->bind(&synth, SynthProperty::pSynthDelayFeedback);
		controls.push_back(value);
	}

	tmp_x = 10;
	tmp_y += 75;

	Button* fm = new Button("FM Algorithm", main_font, 14, tmp_x, tmp_y,  pane_width - 15, 30);
	fm->rect.setFillColor(sf::Color(0, 180, 255));
	fm->set_on_click([&frame, this]{
		frame.change_view(new AnalogSynthFMView(synth, channel, channel_index));
	});
	controls.push_back(fm);


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
