/*
 * AnalogSynthView.h
 *
 *  Created on: 11 Jan 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../../soundengine/asynth.h"
#include "../view/binding.h"

class AnalogSynthView: public ViewController {
private:
	AnalogSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
	std::array<size_t, ANALOG_PART_COUNT> part_sizes = {1, 1, 1, 1, 1, 1, 1, 1};
	std::array<Button*, ANALOG_PART_COUNT> operators;
	Frame* frame = nullptr;
	BindingGUIHandler binder;

	void position_operators();

public:
	AnalogSynthView(AnalogSynth& s, SoundEngineChannel& c, int channel_index);
	virtual Scene create(Frame &frame);
	virtual void update_properties();

	virtual ~AnalogSynthView();
	virtual bool on_action(Control *control);
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_ */
