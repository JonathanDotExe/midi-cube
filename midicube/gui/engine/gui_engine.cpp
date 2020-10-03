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

void Node::draw(int parentX, int parentY, NodeEnv env) {

}


bool Node::collides (int x, int y) {
	return this->x <= x && this->x + this->width > x && this->y <= y && this->y + this->height > y;
}

Node::~Node() {

}

//Parent
Parent::Parent(int x, int y, int width, int height) : Node(x, y, width, height) {

}

void Parent::draw(int parentX, int parentY, NodeEnv env) {
	for (Node* node : children) {
		node->draw(parentX + x, parentY + y, env);
	}
}

void Parent::on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env) {
	x -= this->x;
	y -= this->y;
	for (Node* node : children) {
		if (node->collides(x, y)) {
			node->on_mouse_pressed(x, y, button, env);
		}
	}
}

Node* Parent::traverse_focus(int x, int y) {
	x -= this->x;
	y -= this->y;
	for (Node* node : children) {
		if (node->collides(x, y)) {
			Node* n = node->traverse_focus(x, y);
			if (n) {
				return n;
			}
		}
	}
	return nullptr;
}

void Parent::on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {
	x -= this->x;
	y -= this->y;
	for (Node* node : children) {
		if (node->collides(x, y)) {
			node->on_mouse_released(x, y, button, env);
		}
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

//Frame
Frame::Frame(std::string title, int width, int height) {
	this->title = title;
	this->width = width;
	this->height = height;
	view = nullptr;
	root = nullptr;
}

void Frame::run (ViewController* view) {
	//Init
	InitWindow(width, height, title.c_str());
	SetTargetFPS(30);
	change_view(view);
	//Loop
	while (!WindowShouldClose()) {
		//Events
		Vector2 mouse_pos = GetMousePosition();
		if (root->collides(mouse_pos.x, mouse_pos.y)) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				root->traverse_focus(mouse_pos.x, mouse_pos.y);
				root->on_mouse_pressed(mouse_pos.x, mouse_pos.y, MouseButtonType::LEFT, {focused});
			}
			if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
				root->on_mouse_pressed(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused});
			}
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				root->on_mouse_pressed(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused});
			}

			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				root->on_mouse_released(mouse_pos.x, mouse_pos.y, MouseButtonType::LEFT, {focused});
			}
			if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
				root->on_mouse_released(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused});
			}
			if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
				root->on_mouse_released(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused});
			}
		}
		//Render
		BeginDrawing();
			ClearBackground(RAYWHITE);
			root->draw(0, 0, {focused});
		EndDrawing();
	}
	//Cleanup
	CloseWindow();
}

void Frame::change_view(ViewController* view) {
	delete this->view;
	delete this->root;
	this->view = view;
	this->root = view ? view->init(this) : nullptr;
}

Frame::~Frame() {
	delete this->view;
	delete this->root;
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
	update_style();
}

void Label::draw(int parentX, int parentY, NodeEnv env) {
	positioner.draw(parentX + x, parentY + y, text, style);
}

void Label::update_style() {
	positioner.recalc(width, height, text, style);
}

Label::~Label() {

}

//Button
Button::Button(std::string text, int x, int y, int width, int height) : StyleableNode(x, y, width, height) {
	this->text = text;
	NodeStyle style = get_style();
	style.border_color = BLACK;
	style.border_thickness = 2;
	style.border_radius = 5;
	style.fill_color = LIGHTGRAY;
	set_style(style);
}

void Button::draw(int parentX, int parentY, NodeEnv env) {
	Rectangle rect{parentX + x + .0f, parentY + y + .0f, width + .0f, height + .0f};
	int segments = std::min(width, height);
	//Background
	DrawRectangleRounded(rect, style.border_radius/segments, 1, style.fill_color);
	//Border
	DrawRectangleRoundedLines(rect, style.border_radius/segments, 1, style.border_thickness, style.border_color);
	//Text
	positioner.draw(parentX + x + inner_padding, parentY + y + inner_padding, text, style);
}

void Button::update_style() {
	positioner.recalc(width + inner_padding * 2, height - inner_padding * 2, text, style);
}

Button::~Button() {

}
