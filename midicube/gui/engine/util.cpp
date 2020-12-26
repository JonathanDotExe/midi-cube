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

void center_text_left(sf::Text& text, int x, int y, int height) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(0, bounds.top + bounds.height/2);
	text.setPosition(x, y + height/2);
}

void center_text_right(sf::Text& text, int x, int y, int width, int height) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(0, bounds.top + bounds.height/2);
	text.setPosition(width - bounds.width, y + height/2);
}

void center_text_top(sf::Text& text, int x, int y, int width, int height) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.left + bounds.width/2, 0);
	text.setPosition(x + width/2, y);
}

