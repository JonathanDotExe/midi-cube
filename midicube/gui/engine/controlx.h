/*
 * controlsx.h
 *
 *  Created on: Oct 26, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTROLX_H_
#define MIDICUBE_GUI_ENGINE_CONTROLX_H_

#include "core.h"
#include "util.h"
#include "container.h"
#include "../../util.h"

class OrganDrawbar : public StyleableNode {

private:
	std::function<void (int)> on_change;

	void move_drawbar(int y);

public:
	BoxStyle style;
	BoxStyle button_style;
	int value;

	OrganDrawbar(int value);

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void set_on_change(std::function<void (bool)> on_change);

	FOCUS_TRAVERSABLE

	virtual void on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env);

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion, MouseButtonType button, NodeEnv env);

	virtual Vector get_content_size() {
		return {20, 150};
	}

	virtual ~OrganDrawbar();

};

class OrganSwitch : public StyleableNode {

private:
	std::function<void (bool)> on_change;
	std::string on_text;
	std::string off_text;
	TextPositioner top_positioner;
	TextPositioner bottom_positioner;

public:
	BoxStyle on_style;
	BoxStyle off_style;
	TextStyle text_style;
	bool value;

	OrganSwitch(bool value, std::string on_text = "ON", std::string off_text = "OFF");

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void set_on_change(std::function<void (bool)> on_change);

	FOCUS_TRAVERSABLE

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env);

	virtual Vector get_content_size();

	virtual void update_style();

	virtual ~OrganSwitch();

};


#endif /* MIDICUBE_GUI_ENGINE_CONTROLX_H_ */
