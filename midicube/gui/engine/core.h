/*
 * core.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <SFML/Graphics.hpp>
#include <boost/lockfree/queue.hpp>
#include <functional>
#include "../../util.h"
#include "../../property.h"

#define SELECTABLE virtual bool selectable() const { return true; };

class Frame;

class Control {
protected:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

public:
	Frame* frame = nullptr;

	Control(int x = 0, int y = 0, int width = 0, int height = 0);

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected) = 0;

	bool collides (int x, int y);

	virtual void property_change(PropertyChange change) {

	}

	virtual bool selectable() const {
		return false;
	}

	virtual void on_mouse_pressed(int x, int y, sf::Mouse::Button button) {

	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion) {

	}

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button) {

	}

	virtual ~Control() {

	}

};

class BindableControl : public Control{

protected:

	virtual void bound_property_change(PropertyValue val) = 0;

public:
	PropertyHolder* holder = nullptr;
	size_t property = 0;

	BindableControl(int x = 0, int y = 0, int width = 0, int height = 0) : Control(x, y, width, height) {

	}

	void bind(PropertyHolder* holder, size_t property) {
		this->holder = holder;
		this->property = property;
	}

	virtual void property_change(PropertyChange change) {
		Control::property_change(change);
		if (change.holder == holder && change.property == property) {
			bound_property_change(change.value);
		}
	}

	virtual ~BindableControl() {

	}

};

class ViewController {
public:
	ViewController() {

	}

	virtual std::vector<Control*> create(Frame& frame) = 0;

	virtual ~ViewController() {

	}
};

class Frame {
private:
	int width;
	int height;
	std::string title;
	ViewController* view;
	std::vector<Control*> controls;

	bool mouse_pressed = false;
	bool redraw = true;
	int last_mouse_x = 0;
	int last_mouse_y = 0;
	Control* selected;

	boost::lockfree::queue<std::function<void ()>*> tasks;
public:
	Frame(int width, int height, std::string title);

	void run(ViewController* v);

	void run_task(std::function<void ()> task);

	void request_redraw() {
		redraw = true;
	}

	~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}

};




#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
