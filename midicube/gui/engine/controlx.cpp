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
	get_layout().padding_left = 5;
	get_layout().padding_right = 5;

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

void OrganDrawbar::set_on_change(std::function<void (bool)> on_change) {
	this->on_change = on_change;
}

void OrganDrawbar::move_drawbar(int y) {
	//TODO use organ constants
	int old_val = value;
	value = 8.0 * (y - height/9)/(height - height/9);
	if (value < 0) {
		value = 0;
	}
	else if (value > 8) {
		value = 8;
	}
	if ((int) value != old_val) {
		if (on_change) {
			on_change(value);
		}
		frame->request_redraw();
	}
}

void OrganDrawbar::on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env) {
	if (button == MouseButtonType::LEFT) {
		move_drawbar(y);
	}
}

void OrganDrawbar::on_mouse_drag(int x, int y, int x_motion, int y_motion, MouseButtonType button, NodeEnv env) {
	if (button == MouseButtonType::LEFT) {
		move_drawbar(y);
	}
}

OrganDrawbar::~OrganDrawbar() {

}

//OrganSwitch
OrganSwitch::OrganSwitch(bool value, std::string on_text, std::string off_test) {
	this->value = value;
	this->on_text = on_text;
	this->off_text = off_text;

	text_style.font_size = 8;
	on_style.fill_color = WHITE;
	on_style.hover_color = WHITE;
	off_style.fill_color = LIGHTGRAY;
	off_style.hover_color = LIGHTGRAY;

	get_layout().padding_top = 2;
	get_layout().padding_bottom = 2;
	get_layout().padding_left = 2;
	get_layout().padding_right = 2;
}

void OrganSwitch::draw(int parentX, int parentY, NodeEnv env) {
	std::cout << "Switch" << std::endl;

	if (value) {
		render_box(parentX + x, parentY + y, width, height/2, on_style, env.hovered == this);
		top_positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top, on_text, text_style);
		render_box(parentX + x, parentY + y + height/2, width, height/2, off_style, env.hovered == this);
		top_positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top + height/2, off_text, text_style);
	}
	else {
		render_box(parentX + x, parentY + y, width, height/2, off_style, env.hovered == this);
		top_positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top, on_text, text_style);
		render_box(parentX + x, parentY + y + height/2, width, height/2, on_style, env.hovered == this);
		top_positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top + height/2, off_text, text_style);
	}
}

void OrganSwitch::set_on_change(std::function<void (bool)> on_change) {
	this->on_change = on_change;
}

void OrganSwitch::on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {
	if (env.focused == this && button == MouseButtonType::LEFT) {
		value = !value;
		if (on_change) {
			on_change(value);
		}
		frame->request_redraw();
	}
}

Vector OrganSwitch::get_content_size() {
	Vector top_size = top_positioner.size();
	Vector bottom_size = bottom_positioner.size();
	return {std::max(top_size.x, bottom_size.x), top_size.y + bottom_size.y + layout.padding_top + layout.padding_bottom};
}

void OrganSwitch::update_style() {
	top_positioner.recalc(width - layout.padding_top - layout.padding_bottom, height/2 - layout.padding_top - layout.padding_bottom, on_text, text_style);
	bottom_positioner.recalc(width - layout.padding_top - layout.padding_bottom, height/2 - layout.padding_top - layout.padding_bottom, off_text, text_style);
}

OrganSwitch::~OrganSwitch() {

}

