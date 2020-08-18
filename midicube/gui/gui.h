/*
 * gui.h
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_GUI_H_
#define MIDICUBE_GUI_GUI_H_

#include "model.h"
#include "../soundengine/organ.h"
#include "../soundengine/soundengine.h"
#include <type_traits>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600


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

	virtual bool save_to_history() {
		return true;
	}

	virtual ~View() {

	};
};


class Frame {

private:

	std::vector<View*> history;
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
	int start_x;
	int start_y;
};

struct BindingDragInfo {
	bool dragging;
	bool dialog;
	std::string input_device;
	std::string output_device;
	int input_channel;
	int output_channel;
};

class DevicesMenuView : public View {

private:
	DeviceDragInfo device_drag;
	BindingDragInfo binding_drag;

public:

	DevicesMenuView();

	View* draw();

};


class SoundEngineDeviceMenuView : public View {

private:
	SoundEngineDevice* device;
	std::string options;

public:

	SoundEngineDeviceMenuView(SoundEngineDevice* device);

	View* draw();

};

View* create_view_for_device(AudioDevice* device);

class B3OrganEngineMenuView : public View {
private:
	B3OrganData* data;
	std::array<std::string, ORGAN_DRAWBAR_COUNT> drawbar_titles = {"16'", "5 1/3'", "8'", "4'", "2 2/3'", "2'", "1 3/5'", "1 1/3'", "1"};
	void draw_drawbar (int x, int y, int width, int height, std::array<int, ORGAN_DRAWBAR_COUNT>& drawbars, size_t index);

public:
	B3OrganEngineMenuView(B3OrganData* device);
	View* draw();

};

View* create_view_for_engine(std::string engine_name, SoundEngineData* data);

#endif /* MIDICUBE_GUI_GUI_H_ */
