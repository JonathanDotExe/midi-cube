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
	text.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
}

void Label::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(text);
}

//Pane
void Pane::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
	rect.setSize(sf::Vector2<float>(width, height));
}

void Pane::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(rect);
}

//Button
void Button::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
	rect.setSize(sf::Vector2<float>(width, height));
	center_text(text, x + get_host()->get_x_offset(), y + get_host()->get_y_offset(), width, height);
}

void Button::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(rect);
	window.draw(text);
}

void Button::on_mouse_action() {
	if (on_click) {
		on_click();
	}
}

void Button::update_text(std::string text) {
	this->text.setString(text);
	update_position(get_x(), get_y(), get_width(), get_height());
}

//Slider
void Slider::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	//Slider
	slider_rect.setPosition(x + get_host()->get_x_offset() + width/2 - width * slider_width / 2, y + get_host()->get_y_offset());
	slider_rect.setSize(sf::Vector2<float>(width * slider_width, height));
	//Button
	double range = height * (1 - button_height);
	button_rect.setPosition(x + get_host()->get_x_offset(), y + range * (1 - progress) + get_host()->get_y_offset());
	button_rect.setSize(sf::Vector2<float>(width, height * button_height));

	//Context box and text
	double value = (max - min) * progress + min;
	text.setString(std::to_string(value));

	center_text_top(text, x + get_host()->get_x_offset(), y + height + 5 + get_host()->get_y_offset(), width, height);
	context_rect.setOrigin(text.getOrigin());
	context_rect.setPosition(text.getPosition());
	sf::FloatRect bounds = text.getLocalBounds();
	context_rect.setSize(sf::Vector2<float>(bounds.left + bounds.width + 2, bounds.top + bounds.height + 2));
}

void Slider::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(slider_rect);
	window.draw(button_rect);
	if (selected) {
		window.draw(context_rect);
		window.draw(text);
 	}
}

void Slider::on_mouse_drag(int x, int y, int x_motion, int y_motion) {
	double old_prog = progress;
	progress -= (double)y_motion/get_height();

	if (progress < 0) {
		progress = 0;
	}
	else if (progress > 1) {
		progress = 1;
	}

	if (old_prog != progress) {
		double value = (max - min) * progress + min;
		if (property.is_bound()) {
			property.set(value);
		}
		update_position(get_x(), get_y(), get_width(), get_height());
	}
}

void Slider::update_properties() {
	if (property.is_bound()) {
		property.get([this](double v) {
			progress = fmin(fmax((v - min)/(max - min), 0), 1);
			update_position(get_x(), get_y(), get_width(), get_height());
		});
	}
}

//CheckBox
void CheckBox::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
	rect.setSize(sf::Vector2<float>(width, height));
	inner_rect.setPosition(x + 4 + get_host()->get_x_offset(), y + 4 + get_host()->get_y_offset());
	inner_rect.setSize(sf::Vector2<float>(width - 8, height - 8));

	center_text_left(text, x + width + 5 + get_host()->get_x_offset(), y + get_host()->get_y_offset(), height);
}

void CheckBox::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(rect);
	if (checked) {
		window.draw(inner_rect);
	}
	window.draw(text);
}

void CheckBox::on_mouse_action() {
	checked = !checked;
	if (property.is_bound()) {
		property.set(checked);
	}
	get_host()->request_redraw();
}

void CheckBox::update_properties() {
	if (property.is_bound()) {
		property.get([this](bool v) {
			checked = v;
			get_host()->request_redraw();
		});
	}
}

//ComboBox
void ComboBox::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
	rect.setSize(sf::Vector2<float>(width, height));
	text.setString(values.at(index));
	center_text(text, x + get_host()->get_x_offset(), y + get_host()->get_y_offset(), width, height);
}

void ComboBox::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(rect);
	window.draw(text);
}

void ComboBox::on_mouse_action() {
	index++;
	if (index >= (int) values.size()) {
		index = 0;
	}
	if (property.is_bound()) {
		property.set(index + start_val);
	}
	update_position(get_x(), get_y(), get_width(), get_height());
}

void ComboBox::update_properties() {
	if (property.is_bound()) {
		property.get([this](double v) {
			index = fmax(0, v - start_val);
			update_position(get_x(), get_y(), get_width(), get_height());
		});
	}
}

//OrganSwitch
void OrganSwitch::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	int lower = height/2;
	int upper = height - lower;
	if (checked) {
		activated_rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
		activated_rect.setSize(sf::Vector2<float>(width, upper));

		deactivated_rect.setPosition(x + get_host()->get_x_offset(), y + upper + get_host()->get_y_offset());
		deactivated_rect.setSize(sf::Vector2<float>(width, lower));
	}
	else {
		deactivated_rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
		deactivated_rect.setSize(sf::Vector2<float>(width, upper));

		activated_rect.setPosition(x + get_host()->get_x_offset(), y + upper + get_host()->get_y_offset());
		activated_rect.setSize(sf::Vector2<float>(width, lower));
	}

	center_text(on_text, x + get_host()->get_x_offset(), y, width + get_host()->get_y_offset(), upper);
	center_text(off_text, x + get_host()->get_x_offset(), y + upper + get_host()->get_y_offset(), width, lower);
}

void OrganSwitch::draw(sf::RenderWindow& window, Control* selected) {
	window.draw(activated_rect);
	window.draw(deactivated_rect);
	window.draw(on_text);
	window.draw(off_text);
}

void OrganSwitch::on_mouse_action() {
	checked = !checked;
	if (property.is_bound()) {
		property.set(checked);
	}
	update_position(get_x(), get_y(), get_width(), get_height());
}

void OrganSwitch::update_properties(){
	if (property.is_bound()) {
		property.get([this](bool v) {
			checked = v;
			update_position(get_x(), get_y(), get_width(), get_height());
			get_host()->request_redraw();
		});
	}
}

int ViewContainer::get_width() const {
	return Control::get_width();
}

void ViewContainer::change_view(ViewController *view) {
	delete next_view;
	next_view = view;
	request_redraw();
}

int ViewContainer::get_y_offset() const {
	return get_host()->get_y_offset() + get_y();
}

int ViewContainer::get_x_offset() const {
	return get_host()->get_x_offset() + get_x();
}

void ViewContainer::request_redraw() {
	get_host()->request_redraw();
}

void ViewContainer::add_control(Control *control) {
	ViewHost::add_control(control);
}

int ViewContainer::get_height() const {
	return Control::get_height();
}

void ViewContainer::close() {
	get_host()->close();
}

void ViewContainer::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	Control::on_mouse_released(x, y, button);
	ViewHost::on_mouse_released(x, y, button);
}

bool ViewContainer::selectable() const {
	return false;
}

void ViewContainer::update_position(int x, int y, int width, int height) {
	Control::update_position(x, y, width, height);
	for (Control* control : get_controls()) {
		control->update_position(control->get_x(), control->get_y(), control->get_width(), control->get_height());
	}
}

Control* ViewContainer::on_mouse_pressed(int x, int y,
		sf::Mouse::Button button) {
	Control* control = ViewHost::on_mouse_pressed(x, y, button);
	if (!control) {
		control = Control::on_mouse_pressed(x, y, button);
	}
	return control;
}

void ViewContainer::update_properties() {
	for (Control* control : get_controls()) {
		control->update_properties();
	}
}

void ViewContainer::draw(sf::RenderWindow &window, Control* selected) {
	//Switch view
	if (next_view) {
		switch_view(next_view);
		next_view = nullptr;
	}
	//Draw all
	for (Control* control : get_controls()) {
		control->draw(window, selected);
	}
}

void ViewContainer::notify_remove(Control *control) {
	get_host()->notify_remove(control);
}
