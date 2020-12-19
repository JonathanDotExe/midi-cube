/*
 * control.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "control.h"

//Button
void Button::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x, y);
	rect.setSize(sf::Vector2<float>(width, height));
}

void Button::draw(sf::RenderWindow window) {
	window.draw(rect);
}


