/*
 * core.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "core.h"
#include <iostream>

//Control
Control::Control(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void Control::init(ViewHost* host) {
	if (this->host != nullptr) {
		throw "Control already initialized";
	}
	this->host = host;
	update_properties();
}

void Control::update_position(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	if (host) {
		host->request_redraw();
	}
}

bool Control::collides (int x, int y) {
	return this->x <= x && this->x + this->width > x && this->y <= y && this->y + this->height > y;
}

bool Control::is_visible() const {
	return visible;
}

void Control::set_visible(bool visible) {
	this->visible = visible;
	if (host) {
		host->request_redraw();
	}
}

//ViewHost
void ViewHost::switch_view(ViewController *view) {
	//Init view
	delete this->view;
	this->view = view;
	//Controls
	Scene scene = view->create(*this);
	for (Control* control : get_controls()) {
		remove_control(control);
	}
	for (Control* control : scene.controls) {
		add_control(control);
	}
	view->update_properties();
}

Control* ViewHost::on_mouse_pressed (int x, int y, sf::Mouse::Button button) {
	Control* selected = nullptr;
	for (Control* control : controls) {
		if (control->is_visible() && control->collides(x - get_x_offset(), y - get_y_offset())) {
			selected = control->on_mouse_pressed(x - get_x_offset(), y - get_y_offset(), button);
			if (selected) {
				break; //Only click first control
			}
		}
	}
	return selected;
}

void ViewHost::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	for (Control* control : controls) {
		if (control->is_visible() && control->collides(x - get_x_offset(), y - get_y_offset())) {
			control->on_mouse_released(x - get_x_offset(), y - get_y_offset(), button);
			break;
		}
	}
}
