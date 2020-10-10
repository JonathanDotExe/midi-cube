/*
 * util.cpp
 *
 *  Created on: 10 Oct 2020
 *      Author: jojo
 */

#include "util.h"

//TextPositioner
void TextPositioner::draw(int x, int y, std::string text, NodeStyle& style) {
	DrawText(text.c_str(), x + x_off, y + y_off, style.font_size, style.font_color);
}

void TextPositioner::recalc(int width, int height, std::string text, NodeStyle& style) {
	Vector2 dim = MeasureTextEx(GetFontDefault(), text.c_str(), style.font_size, style.font_size/10);
	this->width = dim.x;
	this->height = dim.y;
	//X
	switch (style.text_halignment) {
	case HorizontalAlignment::LEFT:
		x_off = 0;
		break;
	case HorizontalAlignment::CENTER:
		x_off = width/2 - dim.x/2;
		break;
	case HorizontalAlignment::RIGHT:
		x_off = width - dim.x;
		break;
	}
	//Y
	switch (style.text_valignment) {
	case VerticalAlignment::TOP:
		y_off = 0;
		break;
	case VerticalAlignment::CENTER:
		y_off = height/2 - dim.y/2;
		break;
	case VerticalAlignment::BOTTOM:
		y_off = height - dim.y;
		break;
	}
}

Vector TextPositioner::size() {
	return {width, height};
}