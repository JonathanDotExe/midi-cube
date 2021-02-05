/*
 * core.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <SFML/Graphics.hpp>
#include <boost/lockfree/spsc_queue.hpp>
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
	bool visible = true;

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

	bool is_visible() const;

	void set_visible(bool visible = true);
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
	int last_mouse_x = 0;
	int last_mouse_y = 0;
	Control* selected;

	bool redraw = true;
	ViewController* next_view = nullptr;
	bool request_close = false;

	boost::lockfree::spsc_queue<PropertyChange> changes;

public:
	MidiCube& cube;
	Frame(MidiCube& cube, int width, int height, std::string title);

	void run(ViewController* v);

	virtual void property_change(PropertyChange change);

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

private:
	void switch_view(ViewController* view);

};

class BindableControl : public Control{

protected:

	virtual void bound_property_change(PropertyValue val) = 0;

public:
	PropertyHolder* holder = nullptr;
	size_t property = 0;
	size_t sub_property = 0;

	BindableControl(int x = 0, int y = 0, int width = 0, int height = 0) : Control(x, y, width, height) {

	}

	void bind(PropertyHolder* holder, size_t property, size_t sub_property = 0) {
		this->holder = holder;
		this->property = property;
		this->sub_property = sub_property;
	}

	virtual void property_change(PropertyChange change) {
		Control::property_change(change);
		if (change.holder == holder && change.property == property && change.sub_property == sub_property) {
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
	if (holder) {
		frame->cube.perform_change({holder, property, value, sub_property});
	}
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
