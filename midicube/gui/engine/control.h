/*
 * control.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTROL_H_
#define MIDICUBE_GUI_ENGINE_CONTROL_H_

#include "core.h"
#include <functional>

class Button : public Control {
private:
	std::function<void(void)> on_click = nullptr;

public:
	sf::RectangleShape rect;
	sf::Text text;

	Button(std::string text, sf::Font& font, int text_size, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
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

		this->slider_rect.setFillColor(sf::Color(220, 220, 220));
		this->button_rect.setFillColor(sf::Color::White);

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);

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



#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
