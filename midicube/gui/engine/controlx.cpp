/*
 * controlx.cpp
 *
 *  Created on: Oct 26, 2020
 *      Author: jojo
 */

#include "controlx.h"


//OrganDrawbar
OrganDrawbar::OrganDrawbar(int value) {
	this->value = value;
	get_layout().padding_left = 2;
	get_layout().padding_right = 2;
	get_layout().padding_top = 2;
	get_layout().padding_bottom = 2;

	style.border_color = BLACK;
	style.border_thickness = 1;
	style.border_radius = 0;
	style.fill_color =  BLACK;
	style.hover_color = BLACK;

	button_style.border_color = BLACK;
	button_style.border_thickness = 1;
	button_style.border_radius = 0.1f;
	button_style.fill_color = WHITE;
	button_style.hover_color = WHITE;
}

void OrganDrawbar::draw(int parentX, int parentY, NodeEnv env) {
	int val = value;
	render_box(parentX + x + layout.padding_left, parentY + y, width - layout.padding_left - layout.padding_right, height * val/9, style, env.hovered == this);
	render_box(parentX + x, parentY + y + height * val/9, width, height/9, button_style, env.hovered == this);
}

void OrganDrawbar::set_on_change(std::function<void (double)> on_change) {
	this->on_change = on_change;
}

void OrganDrawbar::move_drawbar(int y) {
	//TODO use organ constants
	std::cout << "Move" << std::endl;

	int old_val = value;
	value = 8 * y/height;
	if (value < 0) {
		value = 0;
	}
	else if (value > 8) {
		value = 8;
	}
	if ((int) value != old_val) {
		std::cout << value << std::endl;
		if (on_change) {
			on_change(value);
		}
		frame->request_redraw();
	}
}

void OrganDrawbar::on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env) {
	if (button == MouseButtonType::LEFT) {
		move_drawbar(y - this->y);
	}
}

void OrganDrawbar::on_mouse_drag(int x, int y, int x_motion, int y_motion, MouseButtonType button, NodeEnv env) {
	if (button == MouseButtonType::LEFT) {
		move_drawbar(y);
	}
}

OrganDrawbar::~OrganDrawbar() {

}
