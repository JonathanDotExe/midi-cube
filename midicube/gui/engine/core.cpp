/*
 * gui_engine.cpp
 *
 *  Created on: 30 Sep 2020
 *      Author: jojo
 */

#include "core.h"


//Node
Node::Node() {
	frame = nullptr;
}

void Node::draw(int parentX, int parentY, NodeEnv env) {

}

void Node::set_frame(Frame* frame) {
	this->frame = frame;
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
	//Wrap content/fit
	if (layout.width == WRAP_CONTENT || (layout.width == MATCH_PARENT && fit)) {
		width = content.x + layout.padding_right + layout.padding_left;
	}
	//Match parent
	else if (layout.width == MATCH_PARENT && !fit) {
		width = parent_width - layout.margin_left - layout.margin_right;
	}
	else {
		width = layout.width;
	}
	//Height
	//Wrap content/fit
	if (layout.height == WRAP_CONTENT || (layout.height == MATCH_PARENT && fit)) {
		height =  content.y + layout.padding_top + layout.padding_bottom;
	}
	//Match parent
	else if (layout.height == MATCH_PARENT && !fit) {
		height = parent_height - layout.margin_top - layout.margin_bottom;
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

void Parent::set_frame(Frame* frame) {
	Node::set_frame(frame);
	for (Node* node : children) {
		node->set_frame(frame);
	}
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
		node->update_layout(width - this->layout.padding_left - this->layout.padding_right, height - this->layout.padding_top - this->layout.padding_bottom);
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
	child->set_frame(frame);
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
		//Selected
		if ((int) mouse_pos.x != last_mouse_x || (int) mouse_pos.y != last_mouse_y) {
			hovered = root->traverse_focus(mouse_pos.x, mouse_pos.y);
			if (focused) {
				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
					focused->on_mouse_drag(mouse_pos.x - last_mouse_x, mouse_pos.y - last_mouse_y, MouseButtonType::LEFT, {focused, hovered});
				}
				if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
					focused->on_mouse_drag(mouse_pos.x - last_mouse_x, mouse_pos.y - last_mouse_y, MouseButtonType::MIDDLE, {focused, hovered});
				}
				if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
					focused->on_mouse_drag(mouse_pos.x - last_mouse_x, mouse_pos.y - last_mouse_y, MouseButtonType::RIGHT, {focused, hovered});
				}
			}
			last_mouse_x = mouse_pos.x;
			last_mouse_y = mouse_pos.y;
		}
		if (root->collides(mouse_pos.x, mouse_pos.y)) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				focused = root->traverse_focus(mouse_pos.x, mouse_pos.y);
				root->on_mouse_pressed(mouse_pos.x, mouse_pos.y, MouseButtonType::LEFT, {focused, hovered});
			}
			if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
				root->on_mouse_pressed(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused, hovered});
			}
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				root->on_mouse_pressed(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused, hovered});
			}

			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				root->on_mouse_released(mouse_pos.x, mouse_pos.y, MouseButtonType::LEFT, {focused, hovered});
			}
			if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
				root->on_mouse_released(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused, hovered});
			}
			if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
				root->on_mouse_released(mouse_pos.x, mouse_pos.y, MouseButtonType::MIDDLE, {focused, hovered});
			}
		}
		//Relayout
		if (relayout) {
			root->update_layout(width, height);
			root->update_position(0, 0);
			relayout = false;
		}
		//Render
		BeginDrawing();
			if (redraw) {
				ClearBackground(RAYWHITE);
				root->draw(0, 0, {focused, hovered});
				redraw = false;
			}
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
		root->set_frame(this);
		root->update_layout(width, height);
		root->update_position(0, 0);
		redraw = true;
	}
}

Frame::~Frame() {
	delete this->view;
	delete this->root;
}



//StylableNode
StyleableNode::StyleableNode() : Node() {

}

StyleableNode::~StyleableNode() {

}

