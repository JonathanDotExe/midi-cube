/*
 * ui.h
 *
 *  Created on: Dec 4, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_UI_H_
#define MIDICUBE_FRAMEWORK_CORE_UI_H_

#include "control.h"
#include "../gui/core.h"
#include <vector>

class Menu {
public:

	ViewController* create_gui_view() = 0;

	ControlView* create_control_view() = 0;

};

class MenuHandler {
private:
	std::vector<Menu*> history;
	Menu* curr_menu = nullptr;

	ControlHost* control_host = nullptr;
	ViewHost* view_host = nullptr;
public:

	MenuHandler(ViewHost* view_host, ControlHost* control_host);

	void change_menu(Menu* menu, bool append_history = true);

	bool back();

	~MenuHandler();

};


#endif /* MIDICUBE_FRAMEWORK_CORE_UI_H_ */