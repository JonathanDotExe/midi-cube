/*
 * core.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <SFML/Graphics.hpp>
#include <functional>
#include <algorithm>
#include "../util/util.h"
#include "../data/data.h"

#define SELECTABLE virtual bool selectable() const { return true; };


//TODO pseudo host for controls
class ViewHost;

class Control {
private:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	bool visible = true;
	ViewHost* host = nullptr;

public:

	Control(int x = 0, int y = 0, int width = 0, int height = 0);

	virtual void init(ViewHost* host);

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, Control* selected) = 0;

	bool collides (int x, int y);

	virtual void update_properties() {

	}

	virtual bool selectable() const {
		return false;
	}

	virtual Control* on_mouse_pressed(int x, int y, sf::Mouse::Button button) {
		return (button == sf::Mouse::Button::Left && selectable()) ? this : nullptr;
	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion) {

	}

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button) {

	}

	virtual void on_mouse_action() {

	}

	virtual ~Control() {

	}

	bool is_visible() const;

	void set_visible(bool visible = true);

	ViewHost* get_host() const {
		return host;
	}

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}

	int get_x() const {
		return x;
	}

	int get_y() const {
		return y;
	}
};

struct Scene {
	std::vector<Control*> controls;
};

class ViewController {
public:
	ViewController() {

	}

	virtual Scene create(ViewHost& frame) = 0;

	virtual void update_properties() {

	}

	virtual bool on_action(Control* control) {
		return true;
	}

	virtual ~ViewController() {

	}
};

class ViewHost {
private:
	ViewController* view = nullptr;
	std::vector<Control*> controls;

public:
	ViewHost() {

	}

	virtual void change_view(ViewController* view) = 0;

	virtual int get_x_offset() const = 0;

	virtual int get_y_offset() const = 0;

	virtual int get_height() const = 0;

	virtual int get_width() const = 0;

	virtual void request_redraw() = 0;

	virtual void notify_remove(Control* control) = 0;

	virtual void close() = 0;

	virtual Control* on_mouse_pressed (int x, int y, sf::Mouse::Button button);

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);

	virtual std::vector<Control*> get_controls() {
		return controls;
	}

	virtual void add_control(Control* control) {
		if (control == nullptr) {
			throw "Can't add nullptr control!";
		}
		else if (std::find(controls.begin(), controls.end(), control) == controls.end()) {
			control->init(this);
			control->update_position(control->get_x(), control->get_y(), control->get_width(), control->get_height());
			controls.push_back(control);
		}
		else {
			throw "Can't add same control to frame twice!";
		}
		request_redraw();
	}

	//Not safe during render loop
	virtual void remove_control(Control* control) {
		for (size_t i = 0; i < controls.size(); ) {
			if (controls[i] == control) {
				controls.erase(controls.begin() + i);
			}
			else {
				++i;
			}
		}
		notify_remove(control);
		delete control;
		request_redraw();
	}

	virtual ~ViewHost() {
		delete view;
		for (Control* control : controls) {
			delete control;
		}
	}

	ViewController* get_view() {
		return view;
	}

protected:
	virtual void switch_view(ViewController* view);

};

class Frame : public ViewHost {
private:
	int width;
	int height;
	std::string title;

	bool mouse_pressed = false;
	int last_mouse_x = 0;
	int last_mouse_y = 0;
	Control* selected;

	bool redraw = true;
	ViewController* next_view = nullptr;
	bool request_close = false;

	bool render_sleep = true;
	std::atomic<bool> update{false};

public:
	Frame(int width, int height, std::string title, bool render_sleep = true);

	void run(ViewController* v);

	void update_properties();

	void propterty_change(void* source, void* prop);

	void request_redraw() {
		redraw = true;
	}

	void close() {
		request_close = true;
	}

	void change_view(ViewController* view) {
		delete next_view;
		next_view = view;
	}

	virtual ~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}

	int get_y_offset() const {
		return 0;
	}

	int get_x_offset() const {
		return 0;
	}

	void notify_remove(Control *control);
};

#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
