/*
 * frame.h
 *
 *  Created on: Nov 3, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_GUI_FRAME_H_
#define MIDICUBE_FRAMEWORK_GUI_FRAME_H_

#include <atomic>
#include "../util/util.h"
#include "../data/data.h"
#include "core.h"

class MenuHandler;

class Frame : public ViewHost {
private:
	int width;
	int height;
	std::string title;

	bool mouse_pressed = false;
	int last_mouse_x = 0;
	int last_mouse_y = 0;
	Control* selected;

	bool redraw = true;
	ViewController* next_view = nullptr;
	bool request_close = false;

	bool render_sleep = true;
	std::atomic<bool> update{false};
	ActionHandler action_handler;

	MenuHandler* menu_handler = nullptr;

public:
	Frame(int width, int height, std::string title, bool render_sleep = true);

	void init_menu_handler(MenuHandler* menu_handler) {
		if (this->menu_handler) {
			throw "Menu Handler already initialized!";
		}
		this->menu_handler = menu_handler;
	}

	virtual void run(Menu* m);

	virtual void update_properties();

	virtual void property_change(void* source, void* prop);

	void request_redraw() {
		redraw = true;
	}

	virtual void close() {
		request_close = true;
	}

	virtual void change_view(ViewController* view) {
		delete next_view;
		next_view = view;
	}

	virtual ~Frame();

	virtual int get_height() const {
		return height;
	}

	virtual int get_width() const {
		return width;
	}

	virtual int get_y_offset() const {
		return 0;
	}

	virtual int get_x_offset() const {
		return 0;
	}

	virtual void notify_remove(Control *control);
	virtual ActionHandler& get_action_handler();
	virtual bool change_menu(Menu *menu, bool append_history=true);
	virtual bool menu_back();

protected:
	virtual void switch_view(ViewController *view);
};


#endif /* MIDICUBE_FRAMEWORK_GUI_FRAME_H_ */
