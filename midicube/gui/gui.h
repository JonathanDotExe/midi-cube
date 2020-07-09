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

void load_gui_resources();


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

	View* draw();

};

class DevicesMenuView : public View {

	View* draw();

};

#endif /* MIDICUBE_GUI_GUI_H_ */
