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
	Button* demo_button = new Button(frame.get_width()/2 - 50, frame.get_height()/2 - 10, 100, 20);
	demo_button->set_on_click([]() {
		std::cout << "Click" << std::endl;
	});
	controls.push_back(demo_button);

	return controls;
}

SoundEngineView::~SoundEngineView() {

}

