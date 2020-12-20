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

	virtual void draw(sf::RenderWindow& window);

	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);

	virtual ~Button() {

	}

	void set_on_click(const std::function<void(void)> &onClick = nullptr) {
		on_click = onClick;
	}
};



#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
