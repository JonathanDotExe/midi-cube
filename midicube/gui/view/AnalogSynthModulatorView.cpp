/*
 * AnalogSynthModultatorView.cpp
 *
 *  Created on: Jan 24, 2021
 *      Author: jojo
 */

#include "AnalogSynthModulatorView.h"
#include "AnalogSynthOscilatorView.h"
#include "AnalogSynthView.h"
#include "resources.h"

AnalogSynthModulatorView::AnalogSynthModulatorView(AnalogSynth &s,
		SoundEngineChannel &c, int channel_index, size_t part) : synth(s), channel(c) {
	this->channel_index = channel_index;
	this->part = part;
}

void AnalogSynthModulatorView::property_change(PropertyChange change) {
}

Scene AnalogSynthModulatorView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<Control*> show_amount;
	std::vector<Control*> show_source;
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

	//Col 1 - Mod Env
	//Title
	{
		Label* title = new Label("Mod Envelope", main_font, 24, tmp_x, tmp_y);
		controls.push_back(title);
	}
	tmp_y += 30;
	//Envelope
	adsr_controls(&controls, tmp_x, tmp_y, part, SynthPartProperty::pSynthEnvAttack);
	tmp_y += 75;
	//Volume
	property_mod_controls(&controls, tmp_x, tmp_y, part, SynthPartProperty::pSynthEnvVolume, "Volume", &show_amount, &show_source);
	tmp_y += 75;

	//Col 2 - LFO
	tmp_x = 500;
	tmp_y = 10;
	//Title
	{
		Label* title = new Label("LFO", main_font, 24, tmp_x, tmp_y);
		controls.push_back(title);
	}
	tmp_y += 30;
	//Frequency
	{
		Label* title = new Label("Frequency", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 50, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul *= 0.25;
		value->bind(part, SynthPartProperty::pSynthLFOFrequency);
		controls.push_back(value);
	}
	tmp_x += 90;
	tmp_y += 15;
	//Wave Form
	{
		std::vector<std::string> waveforms = {"Sine", "Saw Down", "Saw Up", "Square", "Triangle", "Noise"};

		ComboBox* waveform = new ComboBox(1, waveforms, main_font, 16, 0, tmp_x , tmp_y, 150, 40);
		waveform->bind(part, SynthPartProperty::pSynthLFOWaveForm);
		controls.push_back(waveform);
	}
	tmp_x = 500;
	tmp_y += 60;
	//Volume
	property_mod_controls(&controls, tmp_x, tmp_y, part, SynthPartProperty::pSynthLFOVolume, "Volume", &show_amount, &show_source);
	tmp_y += 75;

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

AnalogSynthModulatorView::~AnalogSynthModulatorView() {

}
