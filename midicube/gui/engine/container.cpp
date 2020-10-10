/*
 * container.cpp
 *
 *  Created on: 10 Oct 2020
 *      Author: jojo
 */

#include "container.h"

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
		curr_y += layout.margin_right;

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

//HBox
void HBox::update_layout(int parent_width, int parent_height) {
	Node::update_layout(parent_width, parent_height);
	int curr_x = layout.padding_top;
	for (Node* node : get_children()) {
		NodeLayout& layout = node->get_layout();
		//Position
		curr_x += layout.margin_left;
		node->update_layout(width, height);
		//Alignment
		int y = 0;
		switch (layout.valignment) {
		case VerticalAlignment::TOP:
			y = layout.margin_top;
			break;
		case VerticalAlignment::CENTER:
			y = height/2 - node->get_height()/2;
			break;
		case VerticalAlignment::BOTTOM:
			y = height - layout.margin_bottom - height;
			break;
		}
		node->update_position(curr_x, y);
		curr_x += node->get_width();
		curr_x += layout.margin_bottom;

	}
}

Vector HBox::get_content_size() {
	int curr_x = 0;
	int curr_y = 0;
	for (Node* node : get_children()) {
		Vector size = node->calc_size(0, 0, true);
		curr_x += node->get_layout().margin_left;
		curr_x += size.x;
		curr_x += node->get_layout().margin_right;

		if (curr_y < size.y) {
			curr_y = size.y;
		}
	}
	return {curr_x, curr_y};
}

