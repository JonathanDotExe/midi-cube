/*
 * core.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "core.h"
#include <iostream>

//Control
Control::Control(int x, int y, int width, int height) {
	update_position(x, y, width, height);
}

void Control::update_position(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	if (frame) {
		frame->request_redraw();
	}
}

bool Control::collides (int x, int y) {
	return this->x <= x && this->x + this->width > x && this->y <= y && this->y + this->height > y;
}

bool Control::is_visible() const {
	return visible;
}

void Control::set_visible(bool visible) {
	this->visible = visible;
	if (frame) {
		frame->request_redraw();
	}
}

//Frame
Frame::Frame(MidiCube& c, int width, int height, std::string title, bool render_sleep) : cube(c) {
	this->width = width;
	this->height = height;
	this->title = title;
	this->view = nullptr;
	this->render_sleep = render_sleep;

	this->selected = nullptr;
}

void Frame::run(ViewController* v) {
	//Main loop
	sf::RenderWindow window(sf::VideoMode(width, height), title);
	window.setFramerateLimit(30);
	#ifndef MIDICUBE_NO_WINDOW_ORIGIN
		window.setPosition(sf::Vector2i(0, 0));
	#endif

	//View
	switch_view(v);

	while (window.isOpen()) {
		//Property changes
		if (cube.updated) {
			for (Control* control : controls) {
				control->update_properties();
			}
			view->update_properties();
			cube.updated = false;
		}
		//Execute return actions
		cube.action_handler.execute_return_actions();
		//Events
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
			{
				bool left = event.mouseButton.button == sf::Mouse::Left;
				if (left) {
					mouse_pressed = true;
					last_mouse_x = event.mouseButton.x;
					last_mouse_y = event.mouseButton.y;
				}
				for (Control* control : controls) {
					if (control->is_visible() && control->selectable() && control->collides(event.mouseButton.x, event.mouseButton.y)) {
						control->on_mouse_pressed(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
						if (left) {
							selected = control;
						}
						break;
					}
				}
				request_redraw();
			}
				break;
			case sf::Event::MouseMoved:
				if (mouse_pressed && selected) {
					int x_motion = event.mouseMove.x - last_mouse_x;
					int y_motion = event.mouseMove.y - last_mouse_y;

					selected->on_mouse_drag(event.mouseMove.x, event.mouseMove.y, x_motion, y_motion);
					last_mouse_x = event.mouseMove.x;
					last_mouse_y = event.mouseMove.y;
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					mouse_pressed = false;
					if (selected && selected->collides(event.mouseButton.x, event.mouseButton.y)) {
						selected->on_mouse_action();
					}
					selected = nullptr;
				}
				for (Control* control : controls) {
					if (control->is_visible() && control->selectable() && control->collides(event.mouseButton.x, event.mouseButton.y)) {
						control->on_mouse_released(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
						break;
					}
				}
				request_redraw();
				break;
			default:
				break;
			}
		}
		//Render
		if (redraw || !render_sleep) {
			window.clear(sf::Color(80, 80, 80));
			for (Control* control : controls) {
				if (control->is_visible()) {
					control->draw(window, selected == control);
				}
			}
			redraw = false;
		}
		//Close
		if (request_close) {
			window.close();
			request_close = false;
		}
		window.display();
		//Change view
		if (next_view) {
			//Wait for tasks
			while (cube.action_handler.remaining_realtime_actions()) {
				std::this_thread::yield();
			}
			while (cube.action_handler.remaining_return_actions()) {
				std::this_thread::yield();
			}
			switch_view(next_view);
			next_view = nullptr;
		}
	}
}

void Frame::update_properties() {

}

void Frame::switch_view(ViewController* view) {
	//Init view
	delete this->view;
	this->view = view;
	//Controls
	Scene scene = view->create(*this);
	for (Control* control : controls) {
		delete control;
	}
	controls.clear();
	for (Control* control : scene.controls) {
		control->frame = this;
		control->update_properties();
		controls.push_back(control);
	}
	view->update_properties();
	request_redraw();
}

Frame::~Frame() {
	delete view;
	for (Control* control : controls) {
		delete control;
	}
}

