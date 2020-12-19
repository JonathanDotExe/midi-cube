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
Control::Control(int x = 0, int y = 0, int width = 0, int height = 0) {
	update_position(x, y, width, height);
}

void Control::update_position(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

//Frame
Frame::Frame(int width, int height, std::string title) {
	this->width = width;
	this->height = height;
	this->title = title;
}

void Frame::run() {
	sf::RenderWindow window(sf::VideoMode(width, height), title);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		window.clear();
		//TODO render
		window.display();
	}
}

Frame::~Frame() {

}




#endif /* MIDICUBE_GUI_ENGINE_CORE_CPP_ */
