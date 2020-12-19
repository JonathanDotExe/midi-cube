/*
 * core.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <SFML/Graphics.hpp>

class Frame {
private:
	int width;
	int height;
	std::string title;
public:
	Frame(int width, int height, std::string title);
	void run();
	~Frame();
};


#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
