/*
 * AnalogSynthFMView.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: jojo
 */

#include "../view/AnalogSynthFMView.h"

#include "../view/AnalogSynthOscilatorView.h"
#include "../view/AnalogSynthView.h"

AnalogSynthFMView::AnalogSynthFMView(AdvancedSynth& s) : synth(s) {

}

Scene AnalogSynthFMView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.get_action_handler();

	//Background
	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 5);
	controls.push_back(pane);

	int tmp_x = 10;
	int tmp_y = 10;

	//Title
	Label* carrier = new Label("Carrier", main_font, 18, tmp_x + 85 * ASYNTH_PART_COUNT/2 - 50, tmp_y);
	controls.push_back(carrier);
	tmp_y += 25;

	for (int i = 0; i < ASYNTH_PART_COUNT; ++i) {
		Label* osc = new Label("Osc " + std::to_string(i + 1), main_font, 12, tmp_x + i * 85 + 135, tmp_y);
		controls.push_back(osc);
	}
	tmp_y += 15;

	//Modulator
	Label* modulator = new Label("Modulator", main_font, 18, tmp_x, tmp_y + 45 * ASYNTH_PART_COUNT/2 - 10);
	controls.push_back(modulator);

	tmp_x += 100;

	//Grid
	for (size_t i = 0; i < ASYNTH_PART_COUNT; ++i) {
		OperatorEntity& op = synth.preset.operators.at(i);

		int y = tmp_y + i * 45;
		Label* osc = new Label("Osc " + std::to_string(i + 1), main_font, 12, tmp_x, y + 10);
		controls.push_back(osc);
		for (size_t j = 0; j < ASYNTH_PART_COUNT; ++j) {
			int x = tmp_x + j * 85 + 35;
			DragBox<double>* value = new DragBox<double>(0, 0, 2, main_font, 16, x, y, 80, 40);
			value->border = 0;
			value->property.bind(op.fm.at(j), handler);
			//Color if feedback
			if (i >= j) {
				value->rect.setFillColor(sf::Color(180, 180, 180));
			}
			controls.push_back(value);
		}
		//Audible
		CheckBox* audible = new CheckBox(false, "", main_font, 16, tmp_x + ASYNTH_PART_COUNT * 85 + 35, y, 40, 40);
		audible->property.bind(op.audible, handler);
		controls.push_back(audible);

		//Edit
		Button* edit = new Button("Edit", main_font, 16, tmp_x + ASYNTH_PART_COUNT * 85 + 35 + 45, y, 80, 40);
		edit->rect.setFillColor(sf::Color(0, 180, 255));
		edit->set_on_click([&frame, this, i]{
			AdvancedSynth& s = synth;
			frame.change_menu(new FunctionMenu([s]() { return new AnalogSynthOscilatorView(s, i); }, nullptr));
		});
		controls.push_back(edit);
	}

	//Back Button
	Button* back = new Button("Back", main_font, 18, 5, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.menu_back();
	});
	controls.push_back(back);

	return {controls};
}

AnalogSynthFMView::~AnalogSynthFMView() {

}
