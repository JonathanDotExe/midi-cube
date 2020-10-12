/*
 * control.h
 *
 *  Created on: 10 Oct 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTROL_H_
#define MIDICUBE_GUI_ENGINE_CONTROL_H_

#include "core.h"
#include "util.h"

class Label : public StyleableNode {

private:
	TextPositioner positioner;
	std::string text;

public:
	TextStyle style;

	Label(std::string text);

	virtual Vector get_content_size() {
		return positioner.size();
	}

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void update_style();

	virtual ~Label();

};

class Button : public StyleableNode {

private:
	TextPositioner positioner;
	std::string text;
	std::function<void()> on_click = nullptr;

public:
	BoxStyle style;
	TextStyle text_style;

	Button(std::string text);

	virtual void draw(int parentX, int parentY, NodeEnv env);

	void set_on_click(std::function<void()> on_click);

	virtual Vector get_content_size() {
		return positioner.size();
	}

	FOCUS_TRAVERSABLE

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env);

	virtual void update_style();

	virtual ~Button();

};

class CheckBox : public StyleableNode {

private:
	std::function<void (bool)> on_change;

public:
	BoxStyle style;
	BoxStyle inner_style;
	bool checked = false;

	CheckBox();

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void set_on_change(std::function<void (bool)> on_change);

	FOCUS_TRAVERSABLE

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env);

	virtual Vector get_content_size() {
		return {20, 20};
	}

	virtual ~CheckBox();

};




#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
