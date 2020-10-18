/*
 * util.h
 *
 *  Created on: 10 Oct 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_UTIL_H_
#define MIDICUBE_GUI_ENGINE_UTIL_H_

#include "core.h"

class TextPositioner {
private:
	int x_off = 0;
	int y_off = 0;
	int width = 0;
	int height = 0;
public:
	void draw(int x, int y, std::string text, TextStyle& style);
	void recalc(int width, int height, std::string text, TextStyle& style);
	Vector size();
};

extern void render_box(int x, int y, int width, int height, BoxStyle& style, bool hover = false);

#endif /* MIDICUBE_GUI_ENGINE_UTIL_H_ */