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
#include "../../midicube.h"

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

struct Scene {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> prop_holders;
};

class ViewController {
public:
	ViewController() {

	}

	virtual Scene create(Frame& frame) = 0;

	virtual void property_change(PropertyChange change) {

	}

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
	std::vector<PropertyHolder*> prop_holders;

	bool mouse_pressed = false;
	bool redraw = true;
	int last_mouse_x = 0;
	int last_mouse_y = 0;
	Control* selected;

	boost::lockfree::queue<PropertyChange> changes;
	boost::lockfree::queue<std::function<void ()>*> tasks;

public:
	MidiCube& cube;
	Frame(MidiCube& cube, int width, int height, std::string title);

	void run(ViewController* v);

	void run_task(std::function<void ()> task);

	virtual void property_change(PropertyChange change);

	void request_redraw() {
		redraw = true;
	}

	virtual ~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
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

protected:

	inline void send_change(PropertyValue value);

	inline void send_change(int value);

	inline void send_change(double value);

	inline void send_change(bool value);
};

void BindableControl::send_change(PropertyValue value) {
	frame->cube.perform_change({holder, property, value});
}

void BindableControl::send_change(int value) {
	PropertyValue val;
	val.ival = value;
	send_change(val);
}

void BindableControl::send_change(double value) {
	PropertyValue val;
	val.dval = value;
	send_change(val);
}

void BindableControl::send_change(bool value) {
	PropertyValue val;
	val.bval = value;
	send_change(val);
}


#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
