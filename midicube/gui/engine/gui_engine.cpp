/*
 * gui_engine.cpp
 *
 *  Created on: 30 Sep 2020
 *      Author: jojo
 */

#include "gui_engine.h"


//Node
Node::Node() {

}

void Node::draw(int parentX, int parentY, NodeEnv env) {

}

void Node::update_layout(int parent_width, int parent_height) {
	Vector size = calc_size(parent_width, parent_height, false);
	width = size.x;
	height = size.y;
}

void Node::update_position(int x, int y) {
	this->x = x;
	this->y = y;
}

Vector Node::calc_size(int parent_width, int parent_height, bool fit) {
	Vector content = get_content_size();
	int width = 0;
	int height = 0;
	//Width
	if (layout.width == MATCH_PARENT && !fit) {
		width = parent_width - layout.margin_left - layout.margin_right;
	}
	else if (layout.width == WRAP_CONTENT || (layout.width == MATCH_PARENT && fit)) {
		width = content.x + layout.padding_right + layout.padding_left;
	}
	else {
		width = layout.width;
	}
	//Height
	if (layout.height == MATCH_PARENT && !fit) {
		height = parent_height - layout.margin_top - layout.margin_bottom;
	}
	else if (layout.height == WRAP_CONTENT || (layout.height == MATCH_PARENT && fit)) {
		height =  content.y + layout.padding_top + layout.padding_bottom;
	}
	else {
		height = layout.height;
	}
	return {width, height};
}

bool Node::collides (int x, int y) {
	return this->x <= x && this->x + this->width > x && this->y <= y && this->y + this->height > y;
}

Node::~Node() {

}

//Parent
Parent::Parent() : Node() {

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

Vector Parent::get_content_size() {
	Vector size{0, 0};
	for (Node* node : children) {
		Vector node_size = node->calc_size(0, 0, true);
		if (node_size.x > size.x) {
			size.x = node_size.x;
		}
		if (node_size.y > size.y) {
			size.y = node_size.y;
		}
	}
	return size;
}

void Parent::update_layout(int parent_width, int parent_height) {
	Node::update_layout(parent_width, parent_height);
	for (Node* node : children) {
		node->update_layout(width, height);
		node->update_position(node->get_layout().margin_left, node->get_layout().margin_top);
	}
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

//VBox
void VBox::update_layout(int parent_width, int parent_height) {
	Node::update_layout(parent_width, parent_height);
	int curr_y = layout.padding_left;
	for (Node* node : get_children()) {
		NodeLayout& layout = node->get_layout();
		//Position
		curr_y += layout.margin_top;
		node->update_layout(width, height);
		//Alignment
		int x = 0;
		switch (layout.halignment) {
		case HorizontalAlignment::LEFT:
			x = layout.margin_left;
			break;
		case HorizontalAlignment::CENTER:
			x = width/2 - node->get_width()/2;
			break;
		case HorizontalAlignment::RIGHT:
			x = width - layout.margin_right - width;
			break;
		}
		node->update_position(x, curr_y);
		curr_y += node->get_height();
		curr_y += layout.margin_bottom;

	}
}

Vector VBox::get_content_size() {
	int curr_y = 0;
	int curr_x = 0;
	for (Node* node : get_children()) {
		Vector size = node->calc_size(0, 0, true);
		curr_y += node->get_layout().margin_top;
		curr_y += size.y;
		curr_y += node->get_layout().margin_bottom;

		if (curr_x < size.x) {
			curr_x = size.x;
		}
	}
	return {curr_x, curr_y};
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
				focused = root->traverse_focus(mouse_pos.x, mouse_pos.y);
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
	if (root) {
		root->update_layout(width, height);
		root->update_position(0, 0);
	}
}

Frame::~Frame() {
	delete this->view;
	delete this->root;
}



//StylableNode
StyleableNode::StyleableNode() : Node() {

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
	positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top, text, style);
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
	positioner.recalc(width - layout.padding_left - layout.padding_right, height - layout.padding_top - layout.padding_bottom, text, style);
}

Button::~Button() {

}
