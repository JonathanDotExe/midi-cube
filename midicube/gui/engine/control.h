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

	virtual void update_text(std::string text);

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

template <typename T>
class ComboBox : public StyleableNode {

private:
	TextPositioner positioner;
	std::vector<T> values;
	std::function<std::string(T)> to_string = nullptr;
	std::function<void(T)> on_change = nullptr;
	size_t index = 0;
	std::string text;

public:
	BoxStyle style;
	TextStyle text_style;

	ComboBox(std::vector<T> values, std::function<std::string(T)> to_string);

	virtual void draw(int parentX, int parentY, NodeEnv env);

	void set_on_change(std::function<void(T)> on_change);

	virtual Vector get_content_size() {
		return positioner.size();
	}

	FOCUS_TRAVERSABLE

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env);

	virtual void update_style();

	virtual ~ComboBox();

};

//ComboBox
template<typename T>
ComboBox<T>::ComboBox(std::vector<T> values, std::function<std::string(T)> to_string) : StyleableNode() {
	this->values = values;
	this->to_string = to_string;

	get_layout().width = WRAP_CONTENT;
	get_layout().height = WRAP_CONTENT;
	get_layout().padding_left = 5;
	get_layout().padding_right = 5;
	get_layout().padding_top = 5;
	get_layout().padding_bottom = 5;

	style.border_color = BLACK;
	style.border_thickness = 2;
	style.border_radius = 0.2f;
	style.fill_color = LIGHTGRAY;

	text = to_string(values.at(index));
}

template<typename T>
void ComboBox<T>::draw(int parentX, int parentY, NodeEnv env) {
	render_box(parentX + x, parentY + y, width, height, style, env.hovered == this);
	//Text
	positioner.draw(parentX + x + layout.padding_left, parentY + y + layout.padding_top, text, text_style);
}

template<typename T>
void ComboBox<T>::set_on_change(std::function<void(T)> on_change) {
	this->on_change = on_change;
}

template<typename T>
void ComboBox<T>::on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {
	if (env.focused == this && button == MouseButtonType::LEFT) {
		index++;
		if (index >= values.size()) {
			index = 0;
		}
		text = to_string(values.at(index));
		frame->request_relayout();
		//Actions
		if (on_change) {
			on_change(values.at(index));
		}
	}
}

template<typename T>
void ComboBox<T>::update_style() {
	positioner.recalc(width - layout.padding_left - layout.padding_right, height - layout.padding_top - layout.padding_bottom, text, text_style);
}

template<typename T>
ComboBox<T>::~ComboBox() {

}

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
		return {14, 14};
	}

	virtual ~CheckBox();

};

template <typename T>
class LabeledControl : public HBox {
public:
	T* control;
	Label* label;

	LabeledControl(std::string text, T* control = new T(), bool after = true) {
		layout.width = WRAP_CONTENT;
		layout.height = WRAP_CONTENT;

		this->control = control;
		control->get_layout().valignment = VerticalAlignment::CENTER;
		control->get_layout().margin_left = 2;
		control->get_layout().margin_right = 2;
		label = new Label(text);
		label->get_layout().valignment = VerticalAlignment::CENTER;

		if (after) {
			add_child(control);
			add_child(label);
		}
		else {
			add_child(label);
			add_child(control);
		}
	}

	virtual ~LabeledControl() {

	}
	const T& get_checkbox() const {
		return *control;
	}

	const Label& get_label() const {
		return *label;
	}

};

class Slider : public StyleableNode {

private:
	std::function<void (double)> on_change;

	void move_slider(int y);

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

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion, MouseButtonType button, NodeEnv env);

	virtual void on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env);

	virtual Vector get_content_size() {
		return {30, 100};
	}

	virtual ~Slider();

};

class Spinner : public StyleableNode {

private:
	TextPositioner positioner;
	int min;
	int max;
	int value;
	std::string text;
	std::function<void(int)> on_change = nullptr;

public:
	BoxStyle style;
	BoxStyle button_style;
	TextStyle text_style;
	int button_width = 10;

	Spinner(int min, int max, int value);

	virtual void draw(int parentX, int parentY, NodeEnv env);

	void set_on_change(std::function<void(int)> on_change);

	virtual Vector get_content_size() {
		Vector size = positioner.size();
		size.x += button_width * 2;
		return size;
	}

	FOCUS_TRAVERSABLE

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env);

	virtual void update_style();

	virtual ~Spinner();

};

#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
