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

void Button::draw(sf::RenderWindow& window, bool selected) {
	window.draw(rect);
	window.draw(text);
}

void Button::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	if (on_click) {
		on_click();
	}
}

//Slider
void Slider::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	//Slider
	slider_rect.setPosition(x + width/2 - width * slider_width / 2, y);
	slider_rect.setSize(sf::Vector2<float>(width * slider_width, height));
	//Button
	double range = height * (1 - button_height);
	button_rect.setPosition(x, y + range * (1 - progress));
	button_rect.setSize(sf::Vector2<float>(width, height * button_height));

	//Context box and text
	center_text_top(text, x, y + height + 5, width, height);
	context_rect.setOrigin(text.getOrigin());
	context_rect.setPosition(text.getPosition());
	context_rect.setSize(sf::Vector2<float>(text.getLocalBounds().width, text.getLocalBounds().height));
}

void Slider::draw(sf::RenderWindow& window, bool selected) {
	window.draw(slider_rect);
	window.draw(button_rect);
	if (selected) {
		window.draw(context_rect);
		window.draw(text);
	}
}

void Slider::on_mouse_drag(int x, int y, int x_motion, int y_motion) {
	double old_prog = progress;
	progress -= (double)y_motion/height;

	if (progress < 0) {
		progress = 0;
	}
	else if (progress > 1) {
		progress = 1;
	}

	if (old_prog != progress) {
		update_position(this->x, this->y, width, height);
	}
}


