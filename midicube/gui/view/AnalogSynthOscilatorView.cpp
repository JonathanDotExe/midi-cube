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

static void property_mod_controls(std::vector<Control*>* controls, int x, int y, PropertyHolder* holder, size_t prop, std::string name, std::vector<Control*>* show_amount, std::vector<Control*>* show_source) {
	//Value
	{
		Label* title = new Label(name, main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y + 15, 80, 40);
		value->bind(holder, prop, SynthModulationProperty::pModValue);
		controls->push_back(value);
	}
	x += 90;
	//Mod Env
	{
		Label* title = new Label("Env", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->bind(holder, prop, SynthModulationProperty::pModModEnvAmount);
		controls->push_back(amount);
		show_amount->push_back(amount);

		DragBox<int>* source = new DragBox<int>(0, 0, ANALOG_PART_COUNT, main_font, 16, x, y + 15, 80, 40);
		source->bind(holder, prop, SynthModulationProperty::pModModEnv);
		controls->push_back(source);
		show_source->push_back(source);
	}
	x += 90;
	//LFO
	{
		Label* title = new Label("LFO", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->bind(holder, prop, SynthModulationProperty::pModLFOAmount);
		controls->push_back(amount);
		show_amount->push_back(amount);

		DragBox<int>* source = new DragBox<int>(0, 0, ANALOG_PART_COUNT, main_font, 16, x, y + 15, 80, 40);
		source->bind(holder, prop, SynthModulationProperty::pModLFO);
		controls->push_back(source);
		show_source->push_back(source);
	}
	x += 90;
	//Vel
	{
		Label* title = new Label("Vel", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->bind(holder, prop, SynthModulationProperty::pModVelocityAmount);
		controls->push_back(amount);
	}
	x += 90;
	//CC
	{
		Label* title = new Label("CC", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->bind(holder, prop, SynthModulationProperty::pModCCAmount);
		controls->push_back(amount);
		show_amount->push_back(amount);

		DragBox<int>* source = new DragBox<int>(0, 0, ANALOG_CONTROL_COUNT, main_font, 16, x, y + 15, 80, 40);
		source->bind(holder, prop, SynthModulationProperty::pModCC);
		controls->push_back(source);
		show_source->push_back(source);
	}
	x += 90;
}
Scene AnalogSynthOscilatorView::create(Frame &frame) {
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

	//Col 1
	//Waveform and active
	{
		std::vector<std::string> waveforms = {"Sine", "Saw Down", "Saw Up", "Square", "Triangle", "Noise"};

		ComboBox* waveform = new ComboBox(1, waveforms, main_font, 16, 0, tmp_x , tmp_y, 150, 40);
		waveform->bind(part, SynthPartProperty::pSynthOscWaveForm);
		controls.push_back(waveform);
	}
	{
		CheckBox* active = new CheckBox(false, "Active", main_font, 16, tmp_x + 160, tmp_y, 40, 40);
		active->bind(part, SynthPartProperty::pSynthOscActive);
		controls.push_back(active);
		tmp_y += 50;
	}
	//Reset and audible
	{
		CheckBox* reset = new CheckBox(false, "Reset", main_font, 16, tmp_x, tmp_y, 40, 40);
		reset->bind(part, SynthPartProperty::pSynthOscReset);
		controls.push_back(reset);
	}
	{
		CheckBox* audible = new CheckBox(false, "Audible", main_font, 16, tmp_x + 160, tmp_y, 40, 40);
		audible->bind(part, SynthPartProperty::pSynthOscAudible);
		controls.push_back(audible);
		tmp_y += 50;
	}
	//Randomize and sync
	{
		CheckBox* reset = new CheckBox(false, "Randomize", main_font, 16, tmp_x, tmp_y, 40, 40);
		reset->bind(part, SynthPartProperty::pSynthOscRandomize);
		controls.push_back(reset);
	}
	{
		CheckBox* audible = new CheckBox(false, "Sync", main_font, 16, tmp_x + 160, tmp_y, 40, 40);
		audible->bind(part, SynthPartProperty::pSynthOscSync);
		controls.push_back(audible);
		tmp_y += 50;
	}
	//Volume
	property_mod_controls(&controls, tmp_x, tmp_y, part, SynthPartProperty::pSynthOscVolume, "Volume", &show_amount, &show_source);

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

AnalogSynthOscilatorView::~AnalogSynthOscilatorView() {

}
