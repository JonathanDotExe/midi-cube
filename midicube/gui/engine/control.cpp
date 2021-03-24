/*
 * control.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "control.h"
#include <iostream>
#include <cmath>

//Label
void Label::update_position(int x, int y, int width, int height) {
	text.setPosition(x, y);
}

void Label::draw(sf::RenderWindow& window, bool selected) {
	window.draw(text);
}

//Pane
void Pane::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x, y);
	rect.setSize(sf::Vector2<float>(width, height));
}

void Pane::draw(sf::RenderWindow& window, bool selected) {
	window.draw(rect);
}

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

void Button::update_text(std::string text) {
	this->text.setString(text);
	update_position(x, y, width, height);
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
	double value = (max - min) * progress + min;
	text.setString(std::to_string(value));

	center_text_top(text, x, y + height + 5, width, height);
	context_rect.setOrigin(text.getOrigin());
	context_rect.setPosition(text.getPosition());
	sf::FloatRect bounds = text.getLocalBounds();
	context_rect.setSize(sf::Vector2<float>(bounds.left + bounds.width + 2, bounds.top + bounds.height + 2));
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
		double value = (max - min) * progress + min;
		property.set(value);
		update_position(this->x, this->y, width, height);
	}
}

void Slider::bound_property_change(PropertyValue val) {
	progress = fmin(fmax((val.dval - min)/(max - min), 0), 1);
	update_position(this->x, this->y, width, height);
}

//CheckBox
void CheckBox::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x, y);
	rect.setSize(sf::Vector2<float>(width, height));
	inner_rect.setPosition(x + 4, y + 4);
	inner_rect.setSize(sf::Vector2<float>(width - 8, height - 8));

	center_text_left(text, x + width + 5, y, height);
}

void CheckBox::draw(sf::RenderWindow& window, bool selected) {
	window.draw(rect);
	if (checked) {
		window.draw(inner_rect);
	}
	window.draw(text);
}

void CheckBox::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	if (button == sf::Mouse::Left) {
		checked = !checked;
		property.set(checked);
		frame->request_redraw();
	}
}

void CheckBox::bound_property_change(PropertyValue val) {
	checked = val.bval;
	frame->request_redraw();
}

//ComboBox
void ComboBox::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x, y);
	rect.setSize(sf::Vector2<float>(width, height));
	text.setString(values.at(index));
	center_text(text, x, y, width, height);
}

void ComboBox::draw(sf::RenderWindow& window, bool selected) {
	window.draw(rect);
	window.draw(text);
}

void ComboBox::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	index++;
	if (index >= (int) values.size()) {
		index = 0;
	}
	property.set(index + start_val);
	update_position(this->x, this->y, width, height);
}

void ComboBox::bound_property_change(PropertyValue val) {
	index = val.ival - start_val;
	update_position(this->x, this->y, width, height);
}

//OrganSwitch
void OrganSwitch::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	int lower = height/2;
	int upper = height - lower;
	if (checked) {
		activated_rect.setPosition(x, y);
		activated_rect.setSize(sf::Vector2<float>(width, upper));

		deactivated_rect.setPosition(x, y + upper);
		deactivated_rect.setSize(sf::Vector2<float>(width, lower));
	}
	else {
		deactivated_rect.setPosition(x, y);
		deactivated_rect.setSize(sf::Vector2<float>(width, upper));

		activated_rect.setPosition(x, y + upper);
		activated_rect.setSize(sf::Vector2<float>(width, lower));
	}

	center_text(on_text, x, y, width, upper);
	center_text(off_text, x, y + upper, width, lower);
}

void OrganSwitch::draw(sf::RenderWindow& window, bool selected) {
	window.draw(activated_rect);
	window.draw(deactivated_rect);
	window.draw(on_text);
	window.draw(off_text);
}

void OrganSwitch::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	if (button == sf::Mouse::Left) {
		checked = !checked;
		property.set(checked);
		update_position(this->x, this->y, width, height);
	}
}

void OrganSwitch::bound_property_change(PropertyValue val) {
	checked = val.bval;
	update_position(x, y, width, height);
}

