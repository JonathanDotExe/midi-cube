/*
 * ui.cpp
 *
 *  Created on: Dec 4, 2021
 *      Author: jojo
 */

#include "ui.h"

MenuHandler::MenuHandler(ViewHost *view_host, ControlViewHost *control_host) {
	this->view_host = view_host;
	this->control_host = control_host;
}

void MenuHandler::change_menu(Menu *menu, bool append_history) {
	if (append_history && this->curr_menu) {
		history.push_back(curr_menu);
	}
	else {
		delete curr_menu;
	}

	curr_menu = menu;
	ViewController* gui = curr_menu->create_gui_view();
	if (view_host && gui) {
		view_host->change_view(gui);
	}
	ControlView* control = curr_menu->create_control_view();
	if (control_host && control) {
		control_host->change_control_view(control);
	}
}

bool MenuHandler::back() {
	if (!history.empty()) {
		Menu* next = history.pop_back();
		change_menu(next, false);
		return true;
	}
	return false;
}

void MenuHandler::init(ViewHost *view_host, ControlViewHost *control_host) {
	this->view_host = view_host;
	this->control_host = control_host;
}

MenuHandler::~MenuHandler() {
	delete curr_menu;
	for (Menu* menu : history) {
		delete menu;
	}
}
