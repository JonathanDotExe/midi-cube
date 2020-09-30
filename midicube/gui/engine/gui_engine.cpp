/*
 * gui_engine.cpp
 *
 *  Created on: 30 Sep 2020
 *      Author: jojo
 */

#include "gui_engine.h"

//Node
Node::Node(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void Node::draw(int parentX, int parentY) {

}

Node::~Node() {

}

//Parent
Parent::Parent(int x, int y, int width, int height) : Node(x, y, width, height) {

}

void Parent::draw(int parentX, int parentY) {
	for (Node* node : children) {
		node->draw(parentX + x, parentY + y);
	}
}

void Parent::add_child(Node* child) {
	children.push_back(child);
}

Parent::~Parent() {
	for (Node* node : children) {
		delete node;
	}
	children.clear();
}


//StylableNode
StyleableNode::StyleableNode(int x, int y, int width, int height) : Node(x, y, width, height) {

}

NodeStyle StyleableNode::get_style() {
	return style;
}

void StyleableNode::set_style(NodeStyle style) {
	this->style = style;
	update_style();
}

StyleableNode::~StyleableNode() {

}

//TextPositioner
void TextPositioner::draw(int x, int y, std::string text, NodeStyle& style) {
	DrawText(text.c_str(), x + x_off, y + y_off, style.font_size, style.font_color);
}

void TextPositioner::recalc(int width, int height, std::string text, NodeStyle& style) {
	Vector2 dim = MeasureTextEx(GetFontDefault(), text.c_str(), style.font_size, 0);
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

//Label
Label::Label(std::string text, int x, int y, int width, int height) : StyleableNode(x, y, width, height) {
	this->text = text;
}

void Label::draw(int parentX, int parentY) {
	positioner.draw(parentX + x, parentY + y, text, style);
}

void Label::update_style() {
	positioner.recalc(width, height, text, style);
}

Label::~Label() {

}
