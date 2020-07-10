/*
 * gui.h
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_GUI_H_
#define MIDICUBE_GUI_GUI_H_

#include "model.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480


class View {

private:
	GUIModel* model;

protected:
	GUIModel* get_model();

public:

	void init_model(GUIModel* model);

	/**
	 * For drawing the components and calling events
	 *
	 * @return the new view
	 */
	virtual View* draw() = 0;

	virtual ~View() {

	};
};


class Frame {

private:

	View* view;

public:

	Frame();

	void run ();

	void change_view(View* view);

	~Frame();

};

class MainMenuView : public View {

public:

	View* draw();

};

struct DeviceDragInfo {
	int last_x;
	int last_y;
	bool dragging;
	std::string device;
};

class DevicesMenuView : public View {

private:
	DeviceDragInfo device_drag;

public:

	View* draw();

};

#endif /* MIDICUBE_GUI_GUI_H_ */
