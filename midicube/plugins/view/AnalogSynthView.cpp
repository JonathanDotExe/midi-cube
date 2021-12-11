/*
 * AnalogSynthView.cpp
 *
 *  Created on: 11 Jan 2021
 *      Author: jojo
 */

#include "../view/AnalogSynthView.h"

#include "../view/AnalogSynthFMView.h"
#include "../view/AnalogSynthModulatorView.h"
#include "../view/AnalogSynthOperatorView.h"
#include "../view/AnalogSynthOscilatorView.h"
#include "../../plugins/resources.h"


AnalogSynthView::AnalogSynthView(AdvancedSynth& s) : synth(s), binder{main_font} {

}

Scene AnalogSynthView::create(ViewHost &frame) {
	this->frame = &frame;
	std::vector<Control*> controls;
	ActionHandler& handler = frame.get_action_handler();

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Channels
	int cols = ASYNTH_PART_COUNT;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = 150;
	for (size_t i = 0; i < ASYNTH_PART_COUNT; ++i) {
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10;
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);

		controls.push_back(pane);
	}

	for (size_t i = 0; i < ASYNTH_PART_COUNT; ++i) {
		int x = 10 + pane_width * (i % cols);
		int y = 10;

		//Title
		Label* title = new Label("Part " + std::to_string(i + 1), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Oscilator
		Button* osc = new Button("Oscilator", main_font, 14, x + 5, y + 30,  pane_width - 15, 30);
		osc->rect.setFillColor(sf::Color(0, 180, 255));
		osc->set_on_click([&frame, this, i]{
			AdvancedSynth& synth = this->synth;
			frame.change_menu(VIEW_CONTROL_MENU(new AnalogSynthOscilatorView(synth, i), {
					ControlView* view = new ControlView("Analog Synth Modulation " + i);
					ModEnvelopeEntity& env = synth.preset.mod_envs.at(i);
					LFOEntity& lfo = synth.preset.lfos.at(i);
					view->bind(&env.volume.velocity_amount, ControlType::KNOB, 0, 0);
					view->bind(&env.env.hold, ControlType::KNOB, 1, 0);
					view->bind(&env.env.pre_decay, ControlType::KNOB, 2, 0);
					view->bind(&env.env.attack_hold, ControlType::KNOB, 3, 0);
					view->bind(&lfo.volume.mod_env_amount, ControlType::KNOB, 4, 0);
					view->bind(&lfo.volume.velocity_amount, ControlType::KNOB, 5, 0);
					view->bind(&lfo.clock_value, ControlType::KNOB, 8, 0);

					view->bind(&env.volume.value, ControlType::SLIDER, 0, 0);
					view->bind(&env.env.attack, ControlType::SLIDER, 1, 0);
					view->bind(&env.env.decay, ControlType::SLIDER, 2, 0);
					view->bind(&env.env.sustain, ControlType::SLIDER, 3, 0);
					view->bind(&env.env.release, ControlType::SLIDER, 4, 0);
					view->bind(&lfo.volume.value, ControlType::SLIDER, 5, 0);
					view->bind(&lfo.freq, ControlType::SLIDER, 6, 0);
					view->bind(&lfo.waveform, ControlType::SLIDER, 7, 0);
					view->bind(&lfo.sync_phase, ControlType::SLIDER, 8, 0);

					view->bind(&lfo.sync_master, ControlType::BUTTON, 8, 0);

					view->bind(&env.env.peak_volume, ControlType::SLIDER, 1, 1);
					view->bind(&env.env.decay_volume, ControlType::SLIDER, 2, 1);
					view->bind(&env.env.sustain_time, ControlType::SLIDER, 3, 1);
					view->bind(&env.env.release_volume, ControlType::SLIDER, 4, 1);

					view->init(&synth);
					return view;
				}, &synth, i));
		});
		controls.push_back(osc);
		//Operator
		Button* op = new Button("Operator", main_font, 14, x + 5, y + 70, pane_width - 15, 30);
		op->rect.setFillColor(sf::Color(0, 180, 255));
		op->set_on_click([&frame, this, i]{
			AdvancedSynth& s = synth;
			frame.change_menu(VIEW_MENU(new AnalogSynthOperatorView(s, i), &s, i));
		});
		operators[i] = op;
		controls.push_back(op);
		//Modulators
		Button* mod = new Button("Modulation", main_font, 14, x + 5, y + 110,  pane_width - 15, 30);
		mod->rect.setFillColor(sf::Color(0, 180, 255));
		mod->set_on_click([&frame, this, i]{
			AdvancedSynth& s = synth;
			frame.change_menu(VIEW_MENU(new AnalogSynthModulatorView(s, i), &s, i));
		});
		controls.push_back(mod);
	}

	//Part counts
	int tmp_x = 10;
	int tmp_y = pane_height + 20;
	//Operators
	{
		Label* title = new Label("Operator Count", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<int>* value = new DragBox<int>(0, 0, ASYNTH_PART_COUNT, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->property.bind(synth.preset.op_count, handler);
		controls.push_back(value);
	}
	tmp_x += 90;
	//Mod Env Count
	{
		Label* title = new Label("Env Count", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<int>* value = new DragBox<int>(0, 0, ASYNTH_PART_COUNT, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->property.bind(synth.preset.mod_env_count, handler);
		controls.push_back(value);
	}
	tmp_x += 90;
	{
		Label* title = new Label("LFO Count", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);
		DragBox<int>* value = new DragBox<int>(0, 0, ASYNTH_PART_COUNT, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->property.bind(synth.preset.lfo_count, handler);
		controls.push_back(value);
	}
	tmp_y += 75;
	tmp_x = 10;

	//Polyphony Mode
	//Mono
	{
		CheckBox* mono = new CheckBox(false, "Mono", main_font, 16, tmp_x, tmp_y + 15, 40, 40);
		mono->property.bind(synth.preset.mono, handler);
		controls.push_back(mono);
	}
	tmp_x += 120;
	//Legato
	{
		CheckBox* legato = new CheckBox(false, "Legato", main_font, 16, tmp_x, tmp_y + 15, 40, 40);
		legato->property.bind(synth.preset.legato, handler);
		controls.push_back(legato);
	}
	tmp_x += 120;
	//Portamendo
	{
		Label* title = new Label("Portamendo", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 5, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul /= 5;
		value->property.bind(synth.preset.portamendo, handler);
		controls.push_back(value);
	}
	tmp_x = 10;
	tmp_y += 75;

	///Smooth Aftertouch
	{
		Label* title = new Label("Aftertouch Attack", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 5, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul /= 5;
		value->property.bind(synth.preset.aftertouch_attack, handler);
		controls.push_back(value);
		tmp_x += 120;
	}
	{
		Label* title = new Label("Aftertouch Release", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 5, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul /= 5;
		value->property.bind(synth.preset.aftertouch_release, handler);
		controls.push_back(value);
		tmp_x += 120;
	}
	//Aftertouch Velocity Amount
	{
		Label* title = new Label("Vel. Aftertouch Amt.", main_font, 12, tmp_x, tmp_y);
		controls.push_back(title);

		DragBox<double>* value = new DragBox<double>(0, 0, 5, main_font, 16, tmp_x, tmp_y + 15, 80, 40);
		value->drag_mul /= 5;
		value->property.bind(synth.preset.velocity_aftertouch_amount, handler);
		controls.push_back(value);
		tmp_x += 120;
	}
	//Max Aftertouch
	{
		CheckBox* mono = new CheckBox(false, "Max Aftertouch", main_font, 16, tmp_x, tmp_y + 15, 40, 40);
		mono->property.bind(synth.preset.max_aftertouch, handler);
		controls.push_back(mono);
	}
	tmp_x += 120;

	tmp_x = 10;
	tmp_y += 75;

	Button* fm = new Button("FM Algorithm", main_font, 14, tmp_x, tmp_y,  pane_width - 15, 30);
	fm->rect.setFillColor(sf::Color(0, 180, 255));
	fm->set_on_click([&frame, this]{
		AdvancedSynth& s = synth;
		frame.change_menu(VIEW_MENU(new AnalogSynthFMView(s), &s));
	});
	controls.push_back(fm);
	controls.push_back(binder.create_button(5, frame.get_height() - 40, &frame));

	return {controls};
}

void AnalogSynthView::update_properties() {
	for (size_t i = 0; i < ASYNTH_PART_COUNT; ++i) {
		SpinLock& lock = synth.get_lock();
		lock.lock();
		part_sizes[i] = synth.preset.operators[i].oscilator_count;
		position_operators();
		lock.unlock();
	}
}

void AnalogSynthView::position_operators() {
	int cols = ASYNTH_PART_COUNT;
	int pane_width = (frame->get_width() - 15) / cols; //FIXME
	size_t pos = 0;
	for (size_t i = 0; i < ASYNTH_PART_COUNT; ++i) {
		int x = 10 + pane_width * pos;
		int y = 10;
		Button* op = operators[i];
		op->update_position(x + 5, y + 70, pane_width - 15, 30);
		pos += part_sizes[i];
	}
}

AnalogSynthView::~AnalogSynthView() {

}

bool AnalogSynthView::on_action(Control *control) {
	return binder.on_action(control);
}
