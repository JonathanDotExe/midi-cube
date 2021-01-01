/*
 * control.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTROL_H_
#define MIDICUBE_GUI_ENGINE_CONTROL_H_

#include "core.h"
#include "util.h"
#include <functional>

class Label : public Control {
public:
	sf::Text text;

	Label(std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual ~Label() {

	}

};

class Pane : public Control {

public:
	sf::RectangleShape rect;

	Pane(sf::Color color, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		rect.setFillColor(color);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual ~Pane() {

	}

};

class Button : public Control {
private:
	std::function<void(void)> on_click = nullptr;

public:
	sf::RectangleShape rect;
	sf::Text text;

	Button(std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);
		rect.setFillColor(sf::Color(220, 220, 220));

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);

	void update_text(std::string text);

	SELECTABLE

	virtual ~Button() {

	}

	void set_on_click(const std::function<void(void)> &onClick = nullptr) {
		on_click = onClick;
	}
};

class Slider : public BindableControl {

private:
	double progress;
	double min;
	double max;

	double slider_width;
	double button_height;

public:
	sf::RectangleShape slider_rect;
	sf::RectangleShape button_rect;
	sf::RectangleShape context_rect;
	sf::Text text;

	Slider(double value, double min, double max, sf::Font& font, int x = 0, int y = 0, int width = 0, int height = 0, double slider_width = 0.7, double button_height = 0.15) : BindableControl (x, y, width, height) {
		this->min = min;
		this->max = max;
		this->progress = (value - min)/(max - min);

		this->slider_width = slider_width;
		this->button_height = button_height;

		this->slider_rect.setFillColor(sf::Color(50, 50, 50));
		this->button_rect.setFillColor(sf::Color(220, 220, 220));

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(12);

		this->context_rect.setFillColor(sf::Color::White);
		this->context_rect.setOutlineColor(sf::Color::Black);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion);

	SELECTABLE

	virtual ~Slider() {

	}

protected:
	virtual void bound_property_change(PropertyValue val);
};

template <typename T>
class DragBox : public BindableControl {

private:
	double progress;
	T min;
	T max;

public:
	sf::RectangleShape rect;
	sf::Text text;
	double drag_mul = 0.0025;

	DragBox(T value, T min, T max, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : BindableControl (x, y, width, height) {
		this->min = min;
		this->max = max;
		this->progress = (value - min)/(max - min);

		this->rect.setFillColor(sf::Color(220, 220, 220));

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(text_size);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height) {
		Control::update_position(x, y, width, height);
		rect.setPosition(x, y);
		rect.setSize(sf::Vector2<float>(width, height));
		T value = progress * (max - min) + min;
		text.setString(std::to_string(value));
		center_text(text, x, y, width, height);
	}

	virtual void draw(sf::RenderWindow& window, bool selected) {
		window.draw(rect);
		window.draw(text);
	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion) {
		T old_val = progress * (max - min) + min;
		progress += drag_mul * x_motion;

		if (progress < 0) {
			progress = 0;
		}
		else if (progress > 1) {
			progress = 1;
		}

		if (old_val != (progress * (max - min) + min)) {
			T value = progress * (max - min) + min;
			send_change(value);
			update_position(this->x, this->y, width, height);
		}
	}

	SELECTABLE

	virtual ~DragBox() {

	}

protected:
	virtual void bound_property_change(PropertyValue val) {
		T v = val.get(PropertyType<T>{});
		progress = fmin(fmax((double) (v - min)/(max - min), 0), 1);
		update_position(this->x, this->y, width, height);
	}

};

class CheckBox : public BindableControl {
private:
	bool checked = false;

public:
	sf::RectangleShape rect;
	sf::RectangleShape inner_rect;
	sf::Text text;

	CheckBox(bool checked, std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : BindableControl (x, y, width, height) {
		this->checked = checked;

		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);

		rect.setFillColor(sf::Color(220, 220, 220));
		inner_rect.setFillColor(sf::Color(50, 50, 50));

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);

	SELECTABLE

	virtual ~CheckBox() {

	}

protected:
	virtual void bound_property_change(PropertyValue val);

};

class ComboBox : public BindableControl {
private:
	int start_val = 0;
	int index = 0;
	std::vector<std::string> values;

public:
	sf::RectangleShape rect;
	sf::Text text;

	ComboBox(int value, std::vector<std::string> values, sf::Font& font, int text_size = 12, int start_val = 0, int x = 0, int y = 0, int width = 0, int height = 0) : BindableControl (x, y, width, height) {
		this->start_val = start_val;
		this->index = value - start_val;
		this->values = values;

		this->text.setFont(font);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);
		rect.setFillColor(sf::Color(220, 220, 220));

		update_position(x, y, width, height);
	}

	int get_value() {
		return index + start_val;
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);

	SELECTABLE

	virtual ~ComboBox() {

	}

protected:
	virtual void bound_property_change(PropertyValue val);
};

template<int MAX>
class Drawbar : public BindableControl {

private:
	double progress;

	double slider_width;
	int button_height;

public:
	sf::RectangleShape slider_rect;
	sf::RectangleShape button_rect;
	sf::Text title_text;
	sf::Text text;

	Drawbar(int value, sf::Font& font, std::string title, int x = 0, int y = 0, int width = 0, int height = 0, sf::Color button_color = sf::Color::White, double slider_width = 0.7, int button_height = 60) : BindableControl (x, y, width, height) {
		this->progress = (double) value/MAX;

		this->slider_width = slider_width;
		this->button_height = button_height;

		this->slider_rect.setFillColor(sf::Color::Black);
		this->slider_rect.setOutlineColor(sf::Color::White);
		this->slider_rect.setOutlineThickness(2);
		this->button_rect.setFillColor(button_color);
		this->button_rect.setOutlineColor(sf::Color::Black);
		this->button_rect.setOutlineThickness(2);

		this->title_text.setFont(font);
		this->title_text.setFillColor(sf::Color::Black);
		this->title_text.setCharacterSize(24);
		this->title_text.setString(title);

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(24);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height) {
		Control::update_position(x, y, width, height);
		int value = MAX * progress;
		double real_prog = (double) value/MAX;
		//Slider
		double range = height - button_height;
		slider_rect.setPosition(x + width/2 - width * slider_width / 2, y);
		slider_rect.setSize(sf::Vector2<float>(width * slider_width, real_prog * range - 2));
		//Button
		button_rect.setPosition(x, y + real_prog * range);
		button_rect.setSize(sf::Vector2<float>(width, button_height));

		//Value Text
		text.setString(std::to_string(value));
		center_text_top(text, x, y + height + 5, width, height);

		//Title Text
		center_text_top(title_text, x, y - 40, width, 40);
	}

	virtual void draw(sf::RenderWindow& window, bool selected) {
		window.draw(slider_rect);
		window.draw(button_rect);
		window.draw(text);
		window.draw(title_text);
	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion) {
		int old_val = MAX * progress;
		progress += (double)y_motion/height;

		if (progress < 0) {
			progress = 0;
		}
		else if (progress > 1) {
			progress = 1;
		}

		int value = MAX * progress;
		if (old_val != value) {
			send_change(value);
			update_position(this->x, this->y, width, height);
		}
	}

	SELECTABLE

	virtual ~Drawbar() {

	}

protected:
	virtual void bound_property_change(PropertyValue val) {
		progress = (double) val.ival / MAX;
		update_position(x, y, width, height);
	}
};

class OrganSwitch : public BindableControl {
private:
	bool checked = false;

public:
	sf::RectangleShape activated_rect;
	sf::RectangleShape deactivated_rect;
	sf::Text text;
	sf::Text on_text;
	sf::Text off_text;

	OrganSwitch(bool checked, std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0, std::string on_text="On", std::string off_text="Off", int switch_text_size = 12) : BindableControl (x, y, width, height) {
		this->checked = checked;

		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);

		activated_rect.setFillColor(sf::Color::White);
		activated_rect.setOutlineColor(sf::Color::Black);
		activated_rect.setOutlineThickness(2);
		deactivated_rect.setFillColor(sf::Color(180, 180, 140));
		deactivated_rect.setOutlineColor(sf::Color::Black);
		deactivated_rect.setOutlineThickness(2);

		this->on_text.setFont(font);
		this->on_text.setString(on_text);
		this->on_text.setCharacterSize(switch_text_size);
		this->on_text.setFillColor(sf::Color::Black);

		this->off_text.setFont(font);
		this->off_text.setString(off_text);
		this->off_text.setCharacterSize(switch_text_size);
		this->off_text.setFillColor(sf::Color::Black);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);

	SELECTABLE

	virtual ~OrganSwitch() {

	}

protected:
	virtual void bound_property_change(PropertyValue val);

};


#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
