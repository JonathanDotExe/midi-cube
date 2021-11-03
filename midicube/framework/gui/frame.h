/*
 * frame.h
 *
 *  Created on: Nov 3, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_GUI_FRAME_H_
#define MIDICUBE_FRAMEWORK_GUI_FRAME_H_

#include "../util/util.h"
#include "../data/data.h"
#include "../core/plugin.h"
#include "core.h"

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

	MasterPluginHost& host;

public:
	Frame(int width, int height, std::string title, MasterPluginHost& host, bool render_sleep = true);

	void run(ViewController* v);

	void update_properties();

	void propterty_change(void* source, void* prop);

	void request_redraw() {
		redraw = true;
	}

	void close() {
		request_close = true;
	}

	void change_view(ViewController* view) {
		delete next_view;
		next_view = view;
	}

	virtual ~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}

	int get_y_offset() const {
		return 0;
	}

	int get_x_offset() const {
		return 0;
	}

	void notify_remove(Control *control);
};


#endif /* MIDICUBE_FRAMEWORK_GUI_FRAME_H_ */
