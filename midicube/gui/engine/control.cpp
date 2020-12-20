/*
 * control.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "control.h"
#include "util.h"
#include <iostream>

//Button
void Button::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x, y);
	rect.setSize(sf::Vector2<float>(width, height));
	center_text(text, x, y, width, height);
}

void Button::draw(sf::RenderWindow& window) {
	window.draw(rect);
	window.draw(text);
}

void Button::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	if (on_click) {
		on_click();
	}
}


