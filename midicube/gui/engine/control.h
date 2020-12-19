/*
 * control.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTROL_H_
#define MIDICUBE_GUI_ENGINE_CONTROL_H_

#include "core.h"

class Button : public Control {
private:

public:
	sf::RectangleShape rect;

	Button(int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		rect.setFillColor(sf::Color(220, 220, 220));
		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window);

	virtual ~Button() {

	}
};



#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
