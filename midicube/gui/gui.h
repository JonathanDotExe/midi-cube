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
#include "../soundengine/synthesizer.h"
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

	bool input_channel_editmode = false;
	bool output_channel_editmode = false;
	bool start_note_editmode = false;
	bool end_note_editmode = false;

	int input_channel = -1;
	int output_channel = -1;
	int start_note = 0;
	int end_note = 127;
	bool transfer_channel_aftertouch = true;
	bool transfer_pitch_bend = true;
	bool transfer_cc = true;
	bool transfer_prog_change = true;
	bool transfer_other = true;
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
	bool edit_midi = false;
	std::array<std::string, ORGAN_DRAWBAR_COUNT> drawbar_titles = {"16'", "5 1/3'", "8'", "4'", "2 2/3'", "2'", "1 3/5'", "1 1/3'", "1"};

	std::array<bool, ORGAN_DRAWBAR_COUNT> drawbar_editmode = {false};
	bool rotary_editmode = false;
	bool rotary_speed_editmode = false;

	bool percussion_editmode = false;
	bool percussion_third_harmonic_editmode = false;
	bool percussion_fast_decay_editmode = false;
	bool percussion_soft_editmode = false;

	void draw_drawbar (int x, int y, int width, int height, size_t index);

public:
	B3OrganEngineMenuView(B3OrganData* device);
	View* draw();

};

class SynthesizerEngineMenuView : public View {
private:
	SynthesizerData* data;
	int current_preset = 1;

public:
	SynthesizerEngineMenuView(SynthesizerData* data);
	View* draw();

};

View* create_view_for_engine(std::string engine_name, SoundEngineData* data);

#endif /* MIDICUBE_GUI_GUI_H_ */
