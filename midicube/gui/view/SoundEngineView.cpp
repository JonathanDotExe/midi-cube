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

	//Button
	Button* demo_button = new Button("Text", main_font, 18, frame.get_width()/2 - 80, frame.get_height()/2 - 15, 160, 30);
	demo_button->set_on_click([]() {
		std::cout << "Click" << std::endl;
	});
	controls.push_back(demo_button);

	//Slider
	Slider* slider = new Slider(0.3, 0, 1, main_font, 10, 10, 40, 200);
	controls.push_back(slider);

	return controls;
}

SoundEngineView::~SoundEngineView() {

}

