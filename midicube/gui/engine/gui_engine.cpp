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

//Label
Label::Label(std::string text, int x, int y, int width, int height) : StyleableNode(x, y, width, height) {
	this->text = text;
}

void Label::draw(int parentX, int parentY) {

}

void Label::update_style() {

}

Label::~Label() {

}
