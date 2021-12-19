/*
 * frame.cpp
 *
 *  Created on: Nov 3, 2021
 *      Author: jojo
 */

#include "frame.h"
#include "core.h"
#include "../core/ui.h"

//Frame
Frame::Frame(int width, int height, std::string title, bool render_sleep) : ViewHost() {
	this->width = width;
	this->height = height;
	this->title = title;
	this->render_sleep = render_sleep;
	this->selected = nullptr;
}

void Frame::run(Menu* m) {
	//Main loop
	sf::RenderWindow window(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(30);
	#ifndef MIDICUBE_NO_WINDOW_ORIGIN
		window.setPosition(sf::Vector2i(0, 0));
	#endif

	//View
	change_menu(m, true);
	if (next_view) {
		switch_view(next_view);
		next_view = nullptr;
	}

	while (window.isOpen()) {
		//Property changes
		if (update) {
			update = false;
			for (Control* control : get_controls()) {
				control->update_properties();
			}
			get_view()->update_properties();
		}
		//Execute return actions
		action_handler.execute_return_actions();
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

				selected = on_mouse_pressed(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
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
					control->draw(window, selected);
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
	get_action_handler().wait_till_finished_gui();
}

void Frame::update_properties() {

}

Frame::~Frame() {
	delete next_view;
}

void Frame::notify_remove(Control *control) {
	if (selected == control) {
		selected = nullptr;
	}
}

void Frame::property_change(void *source, void *prop) {
	update = true;
	//TODO check for source and prop
}

void Frame::switch_view(ViewController *view) {
	ViewHost::switch_view(view);
}

ActionHandler& Frame::get_action_handler() {
	return action_handler;
}

bool Frame::change_menu(Menu *menu, bool append_history) {
	if (menu_handler) {
		menu_handler->change_menu(menu, append_history);
		return true;
	}
	return false;
}

bool Frame::menu_back() {
	if (menu_handler) {
		return menu_handler->back();
	}
	return false;
}
