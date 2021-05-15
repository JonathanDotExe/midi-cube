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

std::vector<DragBox<double>*> property_mod_controls(std::vector<Control*>* controls, int x, int y, PropertyModulation& mod, ActionHandler& handler, std::string name, std::vector<Control*>* show_amount, std::vector<Control*>* show_source) {
	std::vector<DragBox<double>*> boxes;
	//Value
	{
		Label* title = new Label(name, main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y + 15, 80, 40);
		value->property.bind(mod.value, handler);
		controls->push_back(value);
		boxes.push_back(value);
	}
	x += 90;
	//Mod Env
	{
		Label* title = new Label("Env", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->drag_mul /= 2;
		amount->border = 0;
		amount->property.bind(mod.mod_env_amount, handler);
		controls->push_back(amount);
		show_amount->push_back(amount);
		boxes.push_back(amount);

		DragBox<int>* source = new DragBox<int>(0, 0, ANALOG_PART_COUNT - 1, main_font, 16, x, y + 15, 80, 40);
		source->property.bind(mod.mod_env, handler);
		controls->push_back(source);
		show_source->push_back(source);
	}
	x += 90;
	//LFO
	{
		Label* title = new Label("LFO", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->drag_mul /= 2;
		amount->border = 0;
		amount->property.bind(mod.lfo_amount, handler);
		controls->push_back(amount);
		show_amount->push_back(amount);
		boxes.push_back(amount);

		DragBox<int>* source = new DragBox<int>(0, 0, ANALOG_PART_COUNT - 1, main_font, 16, x, y + 15, 80, 40);
		source->property.bind(mod.lfo_amount, handler);
		controls->push_back(source);
		show_source->push_back(source);
	}
	x += 90;
	//Vel
	{
		Label* title = new Label("Vel", main_font, 12, x, y);
		show_amount->push_back(title);
		controls->push_back(title);

		Label* at_title = new Label("Aftertouch", main_font, 12, x, y);
		show_source->push_back(at_title);
		controls->push_back(at_title);


		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->drag_mul /= 2;
		amount->border = 0;
		amount->property.bind(mod.velocity_amount, handler);
		controls->push_back(amount);
		show_amount->push_back(amount);
		boxes.push_back(amount);

		DragBox<double>* at_amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		at_amount->drag_mul /= 2;
		at_amount->border = 0;
		at_amount->property.bind(mod.aftertouch_amount, handler);
		controls->push_back(at_amount);
		show_source->push_back(at_amount);
		boxes.push_back(at_amount);
	}
	x += 90;
	//CC
	{
		Label* title = new Label("CC", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* amount = new DragBox<double>(0, -1, 1, main_font, 16, x, y + 15, 80, 40);
		amount->drag_mul /= 2;
		amount->border = 0;
		amount->property.bind(mod.cc_amount, handler);
		controls->push_back(amount);
		show_amount->push_back(amount);
		boxes.push_back(amount);

		DragBox<int>* source = new DragBox<int>(0, 0, ANALOG_CONTROL_COUNT - 1, main_font, 16, x, y + 15, 80, 40);
		source->property.bind(mod.cc, handler);
		controls->push_back(source);
		show_source->push_back(source);
	}
	x += 90;
	return boxes;
}


void adsr_controls(std::vector<Control*>* controls, int x, int y, ADSREnvelopeData& data, ActionHandler& handler) {
	DragBoxScale<double> scale = {
			[](double progress, double min, double max) {
				progress *= progress;
				return progress * (max - min) + min;
			},
			[](double value, double min, double max) {
				return sqrt((value - min)/(max - min));
			}
		};
	//Attack
	{
		Label* title = new Label("Attack", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0.0005, 5, main_font, 16, x, y + 15, 80, 40, scale);
		value->drag_step = 4;
		value->property.bind(data.attack, handler);
		controls->push_back(value);
	}
	x += 90;
	//Decay
	{
		Label* title = new Label("Decay", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 10, main_font, 16, x, y + 15, 80, 40, scale);
		value->drag_step = 4;
		value->property.bind(data.decay, handler);
		controls->push_back(value);
	}
	x += 90;
	//Sustain
	{
		Label* title = new Label("Sustain", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y + 15, 80, 40);
		value->drag_step = 2;
		value->property.bind(data.sustain, handler);
		controls->push_back(value);
	}
	x += 90;
	//Release
	{
		Label* title = new Label("Release", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0.0005, 10, main_font, 16, x, y + 15, 80, 40, scale);
		value->drag_step = 4;
		value->property.bind(data.release, handler);
		controls->push_back(value);
	}
	x += 90;

	x -= 90 * 4;
	y += 75;
	x += 90;

	//Decay
	{
		Label* title = new Label("Peak", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y + 15, 80, 40);
		value->drag_step = 4;
		value->property.bind(data.peak_volume, handler);
		controls->push_back(value);
	}
	x += 90;
	//Sustain
	{
		Label* title = new Label("Sustain Time", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 20, main_font, 16, x, y + 15, 80, 40, scale);
		value->drag_step = 2;
		value->property.bind(data.sustain_time, handler);
		controls->push_back(value);
	}
	x += 90;
	//Release
	{
		Label* title = new Label("Release Vol.", main_font, 12, x, y);
		controls->push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y + 15, 80, 40);
		value->drag_step = 4;
		value->property.bind(data.release_volume, handler);
		controls->push_back(value);
	}
	x += 90;
	x -= 90 * 4;
	y += 75;

	std::vector<std::string> shapes = {"LIN", "ANL", "EXP"};
	//Attack shape
	{
		Label* title = new Label("Attack Shape", main_font, 12, x, y);
		controls->push_back(title);

		ComboBox* shape = new ComboBox(0, shapes, main_font, 16, 0, x , y + 15, 80, 40);
		shape->property.bind(data.attack_shape, handler);
		controls->push_back(shape);
	}
	x += 90;
	//Decay shape
	{
		Label* title = new Label("Decay Shape", main_font, 12, x, y);
		controls->push_back(title);

		ComboBox* shape = new ComboBox(0, shapes, main_font, 16, 0, x, y + 15, 80, 40);
		shape->property.bind(data.decay_shape, handler);
		controls->push_back(shape);
	}
	x += 90;
	x += 90;
	//Release shape
	{
		Label* title = new Label("Release Shape", main_font, 12, x, y);
		controls->push_back(title);

		ComboBox* shape = new ComboBox(0, shapes, main_font, 16, 0, x, y + 15, 80, 40);
		shape->property.bind(data.release_shape, handler);
		controls->push_back(shape);
	}
	x += 90;
}

Scene AnalogSynthOscilatorView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<Control*> show_amount;
	std::vector<Control*> show_source;

	ActionHandler& handler = frame.cube.action_handler;
	OscilatorEntity& osc = synth.preset.oscilators.at(this->part);

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	int tmp_x = 10;
	int tmp_y = 10;

	//Col 1
	//Waveform
	{
		std::vector<std::string> waveforms = {"Sine", "Saw Down", "Saw Up", "Square", "Triangle", "Noise"};

		ComboBox* waveform = new ComboBox(1, waveforms, main_font, 16, 0, tmp_x , tmp_y, 150, 40);
		waveform->property.bind(osc.waveform, handler);
		controls.push_back(waveform);
	}
	//Fixed Freq
	{
		CheckBox* audible = new CheckBox(false, "Fixed", main_font, 16, tmp_x + 160, tmp_y, 40, 40);
		audible->property.bind(osc.fixed_freq, handler);
		controls.push_back(audible);
	}
	//Note
	{
		DragBox<unsigned int>* value = new DragBox<unsigned int>(0, 0, 127, main_font, 16, tmp_x + 320, tmp_y, 80, 40);
		value->property.bind(osc.note, handler);
		controls.push_back(value);
	}
	tmp_y += 50;
	//Reset
	{
		CheckBox* reset = new CheckBox(false, "Reset", main_font, 16, tmp_x, tmp_y, 40, 40);
		reset->property.bind(osc.reset, handler);
		controls.push_back(reset);
	}
	//Phase
	{
		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x + 160, tmp_y, 80, 40);
		value->property.bind(osc.phase, handler);
		controls.push_back(value);
	}
	tmp_y += 50;
	//Randomize and sync
	{
		CheckBox* reset = new CheckBox(false, "Randomize", main_font, 16, tmp_x, tmp_y, 40, 40);
		reset->property.bind(osc.randomize, handler);
		controls.push_back(reset);
	}
	{
		CheckBox* audible = new CheckBox(false, "Sync", main_font, 16, tmp_x + 160, tmp_y, 40, 40);
		audible->property.bind(osc.sync, handler);
		controls.push_back(audible);
		tmp_y += 50;
	}
	//Volume
	property_mod_controls(&controls, tmp_x, tmp_y, osc.volume, handler, "Volume", &show_amount, &show_source);
	tmp_y += 75;

	//Unison
	{
		Label* title = new Label("Unison", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<int>* value = new DragBox<int>(0, 0, 7, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->property.bind(osc.unison_amount, handler);
		controls.push_back(value);
	}
	tmp_x += 90;
	//Semi
	{
		Label* title = new Label("Semi", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<int>* value = new DragBox<int>(0, -48, 48, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->property.bind(osc.semi, handler);
		controls.push_back(value);
	}
	tmp_x += 90;
	//Ratio
	{
		Label* title = new Label("Ratio", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 50, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul /= 10;
		value->property.bind(osc.transpose, handler);
		controls.push_back(value);
	}
	tmp_x += 90;

	//Col 2
	tmp_x = 500;
	tmp_y = 10;
	//Pulse Width
	property_mod_controls(&controls, tmp_x, tmp_y, osc.pulse_width, handler, "Shape/PW", &show_amount, &show_source);
	tmp_y += 75;
	//Pitch
	for (auto c : property_mod_controls(&controls, tmp_x, tmp_y, osc.pitch, handler, "Pitch", &show_amount, &show_source)) {
		c->to_string = [](double val) {
			return std::to_string(val >= 0 ? PITCH_SCALE.value(val) : -PITCH_SCALE.value(-val));
		};
		break; //FIXME
	}
	tmp_y += 75;
	//Sync
	for (auto c : property_mod_controls(&controls, tmp_x, tmp_y, osc.sync_mul, handler, "Sync", &show_amount, &show_source)) {
		c->to_string = [](double val) {
			return std::to_string(val >= 0 ? SYNC_SCALE.value(val) : -SYNC_SCALE.value(-val));
		};
		break; //FIXME
	}
	tmp_y += 75;
	//Unison Detune
	for (auto c : property_mod_controls(&controls, tmp_x, tmp_y, osc.unison_detune, handler, "Unison Det.", &show_amount, &show_source)) {
		c->to_string = [](double val) {
			return std::to_string(val >= 0 ? UNISON_DETUNE_SCALE.value(val) : -UNISON_DETUNE_SCALE.value(-val));
		};
		break; //FIXME
	}
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

	return {controls};
}

AnalogSynthOscilatorView::~AnalogSynthOscilatorView() {

}
