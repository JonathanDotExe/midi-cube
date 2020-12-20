/*
 * util.cpp
 *
 *  Created on: Dec 20, 2020
 *      Author: jojo
 */

#include "util.h"

void center_text(sf::Text& text, int x, int y, int width, int height) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.left + bounds.width/2,  bounds.top + bounds.height/2);
	text.setPosition(x + width/2, y + height/2);
}

