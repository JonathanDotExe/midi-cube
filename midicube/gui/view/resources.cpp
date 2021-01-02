/*
 * resources.cpp
 *
 *  Created on: Dec 20, 2020
 *      Author: jojo
 */
#include "resources.h"

sf::Font main_font;

void load_resources () {
	if (!main_font.loadFromFile("./resources/fonts/LiberationSans-Regular.ttf")) {
		throw std::runtime_error("Couldn't load default font!");
	}
}

