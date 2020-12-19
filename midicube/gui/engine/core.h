/*
 * core.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <SFML/Graphics.hpp>

class Frame;

class Control {
protected:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

public:
	Frame* frame = nullptr;

	Control(int x = 0, int y = 0, int width = 0, int height = 0) {}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw (sf::RenderWindow window) = 0;

	virtual ~Control() {

	}

};

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
