/*
 * core.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_CPP_
#define MIDICUBE_GUI_ENGINE_CORE_CPP_

#include "core.h"

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
