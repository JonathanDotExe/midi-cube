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

public:
	BoxStyle style;
	BoxStyle button_style;
	double value;

	OrganDrawbar(int value);

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void set_on_change(std::function<void (double)> on_change);

	FOCUS_TRAVERSABLE

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion, MouseButtonType button, NodeEnv env);

	virtual Vector get_content_size() {
		return {40, 150};
	}

	virtual ~OrganDrawbar();

};


#endif /* MIDICUBE_GUI_ENGINE_CONTROLX_H_ */
