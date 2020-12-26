/*
 * SoundEngineView.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "SoundEngineView.h"

#include <iostream>

SoundEngineView::SoundEngineView() : ViewController() {

}

std::vector<Control*> SoundEngineView::create(Frame& frame) {
	std::vector<Control*> controls;

	//Label
	Label* label = new Label("Demo", main_font, 24, 10, 300, 0, 0);
	label->text.setFillColor(sf::Color::White);
	controls.push_back(label);

	//Button
	Button* demo_button = new Button("Text", main_font, 12, frame.get_width()/2 - 80, frame.get_height()/2 - 15, 160, 30);
	demo_button->set_on_click([]() {
		std::cout << "Click" << std::endl;
	});
	controls.push_back(demo_button);

	//Slider
	Slider* slider = new Slider(0.3, 0, 1, main_font, 10, 10, 40, 200);
	controls.push_back(slider);

	//DragBox
	DragBox<int>* int_box = new DragBox<int>(5, 0, 12, main_font, 18, 80, 10, 60, 40);
	controls.push_back(int_box);

	DragBox<double>* double_box = new DragBox<double>(2, 0, 12, main_font, 18, 150, 10, 80, 40);
	controls.push_back(double_box);

	//Check Box
	CheckBox* check_box = new CheckBox(true, "Demo Checkbox", main_font, 12, 250, 10, 30, 30);
	controls.push_back(check_box);

	return controls;
}

SoundEngineView::~SoundEngineView() {

}

