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
#include "container.h"
#include "../../util.h"

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

class TextCheckBox : public HBox {
public:
	CheckBox* checkbox;
	Label* label;

	TextCheckBox(std::string text) {
		layout.width = WRAP_CONTENT;
		layout.height = WRAP_CONTENT;

		checkbox = new CheckBox();
		checkbox->get_layout().valignment = VerticalAlignment::CENTER;
		checkbox->get_layout().margin_right = 2;
		label = new Label(text);
		label->get_layout().valignment = VerticalAlignment::CENTER;
		add_child(checkbox);
		add_child(label);
	}

	virtual ~TextCheckBox() {

	}
	const CheckBox& get_checkbox() const {
		return *checkbox;
	}

	const Label& get_label() const {
		return *label;
	}

};

class Slider : public StyleableNode {

private:
	std::function<void (double)> on_change;

public:
	BoxStyle style;
	BoxStyle button_style;
	double progress;
	FixedScale scale{0, {}, 1};
	int inner_width = 10;
	int button_height = 10;

	Slider(double value, FixedScale scale);

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void set_on_change(std::function<void (double)> on_change);

	FOCUS_TRAVERSABLE

	virtual void on_mouse_drag(int x_motion, int y_motion, MouseButtonType button, NodeEnv env);

	virtual Vector get_content_size() {
		return {30, 100};
	}

	virtual ~Slider();

};


#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
