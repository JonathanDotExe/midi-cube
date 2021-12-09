/*
 * ui.h
 *
 *  Created on: Dec 4, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_UI_H_
#define MIDICUBE_FRAMEWORK_CORE_UI_H_

#include "control.h"
#include "../gui/frame.h"
#include <vector>
#include <functional>


class Menu {
public:

	virtual ViewController* create_gui_view() = 0;

	virtual ControlView* create_control_view() = 0;

	virtual ~Menu() {

	}

};

class FunctionMenu : public Menu {
public:
	std::function<ViewController* ()> gui;
	std::function<ControlView* ()> control;

	FunctionMenu(std::function<ViewController* ()> g, std::function<ControlView* ()> c) : gui(g), control(c) {

	}

	ViewController* create_gui_view() {
		return gui ? gui() : nullptr;
	}

	ControlView* create_control_view() {
		return control ? control : nullptr;
	}

};

class MenuHandler {
private:
	std::vector<Menu*> history;
	Menu* curr_menu = nullptr;

	ControlViewHost* control_host = nullptr;
	Frame* view_host = nullptr;
public:

	MenuHandler();

	void init(Frame* view_host, ControlViewHost* control_host);

	void change_menu(Menu* menu, bool append_history = true);

	void realtime_process();

	bool back();

	void propterty_change(void* source, void* prop);

	~MenuHandler();

};


#endif /* MIDICUBE_FRAMEWORK_CORE_UI_H_ */
