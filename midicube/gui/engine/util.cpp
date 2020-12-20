/*
 * util.cpp
 *
 *  Created on: Dec 20, 2020
 *      Author: jojo
 */

#include "util.h"

void center_text(sf::Text& text, int x, int y, int width, int height) {
	int w = text.getLocalBounds().width;
	int h = text.getLocalBounds().height;

	text.setPosition(x + width/2 - w/2, y + height/2 - h/2);
}

