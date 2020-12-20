/*
 * core.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_CPP_
#define MIDICUBE_GUI_ENGINE_CORE_CPP_

#include "core.h"

//Control
Control::Control(int x, int y, int width, int height) {
	update_position(x, y, width, height);
}

void Control::update_position(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

bool Control::collides (int x, int y) {
	return this->x <= x && this->x + this->width > x && this->y <= y && this->y + this->height > y;
}

//Frame
Frame::Frame(int width, int height, std::string title) {
	this->width = width;
	this->height = height;
	this->title = title;
	this->view = nullptr;
}

void Frame::run(ViewController* v) {
	//Init view
	this->view = v;
	for (Control* control : view->create(*this)) {
		control->frame = this;
		controls.push_back(control);
	}
	//Main loop
	sf::RenderWindow window(sf::VideoMode(width, height), title);

	while (window.isOpen()) {
		//Events
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				for (Control* control : controls) {
					if (control->collides(event.mouseButton.x, event.mouseButton.y)) {
						control->on_mouse_pressed(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
					}
				}
				break;
			case sf::Event::MouseButtonReleased:
				for (Control* control : controls) {
					if (control->collides(event.mouseButton.x, event.mouseButton.y)) {
						control->on_mouse_released(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
					}
				}
				break;
			default:
				break;
			}
		}
		//Render
		window.clear();
		for (Control* control : controls) {
			control->draw(window);
		}
		window.display();
	}
}

Frame::~Frame() {
	delete view;
	for (Control* control : controls) {
		delete control;
	}
}




#endif /* MIDICUBE_GUI_ENGINE_CORE_CPP_ */
