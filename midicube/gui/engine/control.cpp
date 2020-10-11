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
	update_style();
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
	style.border_radius = 5;
	style.fill_color = LIGHTGRAY;
}

void Button::draw(int parentX, int parentY, NodeEnv env) {
	Rectangle rect{parentX + x + .0f, parentY + y + .0f, width + .0f, height + .0f};
	int segments = std::min(width, height);
	//Background
	DrawRectangleRounded(rect, style.border_radius/segments, 1, style.fill_color);
	//Border
	DrawRectangleRoundedLines(rect, style.border_radius/segments, 1, style.border_thickness, style.border_color);
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
