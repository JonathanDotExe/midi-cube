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

	SELECTABLE

	virtual ~Button() {

	}

	void set_on_click(const std::function<void(void)> &onClick = nullptr) {
		on_click = onClick;
	}
};

class Slider : public Control {

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

	Slider(double value, double min, double max, sf::Font& font, int x = 0, int y = 0, int width = 0, int height = 0, double slider_width = 0.7, double button_height = 0.15) : Control (x, y, width, height) {
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

};

template <typename T>
class DragBox : public Control {

private:
	double progress;
	T min;
	T max;

public:
	sf::RectangleShape rect;
	sf::Text text;
	double drag_mul = 0.0025;

	DragBox(T value, T min, T max, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
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
			update_position(this->x, this->y, width, height);
		}
	}

	SELECTABLE

	virtual ~DragBox() {

	}

};

class CheckBox : public Control {
private:
	bool checked = false;

public:
	sf::RectangleShape rect;
	sf::RectangleShape inner_rect;
	sf::Text text;

	CheckBox(bool checked, std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
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

};



#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
