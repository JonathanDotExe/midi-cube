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

void Control::init(ViewHost* host) {
	if (this->host != nullptr) {
		throw "Control already initialized";
	}
	this->host = host;
	update_properties();
}

void Control::update_position(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	if (host) {
		host->request_redraw();
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
	if (host) {
		host->request_redraw();
	}
}

//ViewHost
void ViewHost::switch_view(ViewController *view) {
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
		add_control(control);
	}
	view->update_properties();
}

Control* ViewHost::on_mouse_pressed (int x, int y, sf::Mouse::Button button) {
	Control* selected = nullptr;
	for (Control* control : controls) {
		if (control->is_visible() && control->collides(x - get_x_offset(), y - get_y_offset())) {
			selected = control->on_mouse_pressed(x - get_x_offset(), y - get_y_offset(), button);
			break; //Only click first control
		}
	}
	return selected;
}

void ViewHost::on_mouse_released(int x, int y, sf::Mouse::Button button) {
	for (Control* control : controls) {
		if (control->is_visible() && control->collides(x - get_x_offset(), y - get_y_offset())) {
			control->on_mouse_released(x - get_x_offset(), y - get_y_offset(), button);
			break;
		}
	}
}

//Frame
Frame::Frame(int width, int height, std::string title, bool render_sleep) : ViewHost(){
	this->width = width;
	this->height = height;
	this->title = title;
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
		/*
		TODO property updates
		for (Control* control : controls) {
			control->update_properties();
		}
		view->update_properties();
		*/
		//Events
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
			{
				if (event.mouseButton.button == sf::Mouse::Left) {
					mouse_pressed = true;
					last_mouse_x = event.mouseButton.x;
					last_mouse_y = event.mouseButton.y;
				}

				on_mouse_pressed(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
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
					if (selected) {
						ViewController* view = selected->get_host()->get_view();
						if (view && selected->collides(event.mouseButton.x - selected->get_host()->get_x_offset(), event.mouseButton.y - selected->get_host()->get_y_offset()) && view->on_action(selected)) {
							selected->on_mouse_action();
						}
					}
					selected = nullptr;
				}

				on_mouse_released(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
				request_redraw();
				break;
			default:
				break;
			}
		}
		//Render
		if (redraw || !render_sleep) {
			window.clear(sf::Color(80, 80, 80));
			for (Control* control : get_controls()) {
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
			switch_view(next_view);
			next_view = nullptr;
		}
	}
}

void Frame::update_properties() {

}

Frame::~Frame() {
	delete next_view;
}

