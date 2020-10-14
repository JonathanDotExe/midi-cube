/*
 * control.cpp
 *
 *  Created on: 10 Oct 2020
 *      Author: jojo
 */

#include "control.h"

//Label
Label::Label(std::string text) : StyleableNode() {
	this->text = text;
	get_layout().width = WRAP_CONTENT;
	get_layout().height = WRAP_CONTENT;
}

void Label::draw(int parentX, int parentY, NodeEnv env) {
	positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top, text, style);
}

void Label::update_style() {
	positioner.recalc(width - layout.padding_left - layout.padding_right, height - layout.padding_top - layout.padding_bottom, text, style);
}

Label::~Label() {

}

//Button
Button::Button(std::string text) : StyleableNode() {
	this->text = text;

	get_layout().width = WRAP_CONTENT;
	get_layout().height = WRAP_CONTENT;
	get_layout().padding_left = 5;
	get_layout().padding_right = 5;
	get_layout().padding_top = 5;
	get_layout().padding_bottom = 5;

	style.border_color = BLACK;
	style.border_thickness = 2;
	style.border_radius = 0.2f;
	style.fill_color = LIGHTGRAY;
}

void Button::draw(int parentX, int parentY, NodeEnv env) {
	render_box(parentX + x, parentY + y, width, height, style, env.hovered == this);
	//Text
	positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top, text, text_style);
}

void Button::set_on_click(std::function<void()> on_click) {
	this->on_click = on_click;
}

void Button::on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {
	if (env.focused == this && button == MouseButtonType::LEFT) {
		//Actions
		if (on_click) {
			on_click();
		}
	}
}

void Button::update_style() {
	positioner.recalc(width - layout.padding_left - layout.padding_right, height - layout.padding_top - layout.padding_bottom, text, text_style);
}

Button::~Button() {

}

//CheckBox
CheckBox::CheckBox() {
	get_layout().width = WRAP_CONTENT;
	get_layout().height = WRAP_CONTENT;
	get_layout().padding_left = 2;
	get_layout().padding_right = 2;
	get_layout().padding_top = 2;
	get_layout().padding_bottom = 2;

	style.border_color = BLACK;
	style.border_thickness = 1;
	style.border_radius = 0;
	style.fill_color = RAYWHITE;
	style.hover_color = LIGHTGRAY;

	inner_style.fill_color = {50, 50, 110, 255};
	inner_style.hover_color = BLACK;
	inner_style.border_radius = 0;
};

void CheckBox::draw(int parentX, int parentY, NodeEnv env) {
	render_box(parentX + x, parentY + y, width, height, style, env.hovered == this);
	if (checked) {
		render_box(parentX + x + layout.padding_left, parentY + y + layout.padding_top, width - layout.padding_left - layout.padding_right, height - layout.padding_top - layout.padding_bottom, inner_style, env.hovered == this);
	}
}

void CheckBox::set_on_change(std::function<void (bool)> on_change) {
	this->on_change = on_change;
}

void CheckBox::on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {
	if (env.focused == this && button == MouseButtonType::LEFT) {
		checked = !checked;
		//Actions
		if (on_change) {
			on_change(checked);
		}
	}
}

CheckBox::~CheckBox() {

}

//Slider
Slider::Slider(double value, FixedScale scale) {
	this->scale = scale;
	progress = this->scale.progress(value);
	get_layout().padding_left = 2;
	get_layout().padding_right = 2;
	get_layout().padding_top = 2;
	get_layout().padding_bottom = 2;

	style.border_color = BLACK;
	style.border_thickness = 1;
	style.border_radius = 1;
	style.fill_color =  RAYWHITE;
	style.hover_color = LIGHTGRAY;

	button_style.border_color = BLACK;
	button_style.border_thickness = 1;
	button_style.border_radius = 0.2f;
	button_style.fill_color = LIGHTGRAY;
	button_style.hover_color = GRAY;
}

void Slider::draw(int parentX, int parentY, NodeEnv env) {
	render_box(parentX + x + (width - inner_width)/2 , parentY + y, inner_width, height, style, env.hovered == this);
	int button_height = height - layout.padding_top - layout.padding_bottom - this->button_height;
	render_box(parentX + x, parentY + y + layout.padding_top + button_height * (1 - progress), width, this->button_height, button_style, env.hovered == this);
}

void Slider::set_on_change(std::function<void (double)> on_change) {
	this->on_change = on_change;
}

void Slider::on_mouse_drag(int x_motion, int y_motion, MouseButtonType button, NodeEnv env) {
	if (button == MouseButtonType::LEFT) {
		double change = (double) y_motion/height;
		double old_prog = progress;
		progress -= change;
		if (progress < 0) {
			progress = 0;
		}
		else if (progress > 1) {
			progress = 1;
		}
		if (progress != old_prog && on_change) {
			on_change(scale.value(progress));
		}
	}
}

Slider::~Slider() {

}

