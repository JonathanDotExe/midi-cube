/*
 * core.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <SFML/Graphics.hpp>

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
public:
	Frame(int width, int height, std::string title);

	void run(ViewController* v);

	~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}

	void request_redraw() {
		redraw = true;
	}
};




#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
