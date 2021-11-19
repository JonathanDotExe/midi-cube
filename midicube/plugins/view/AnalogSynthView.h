/*
 * AnalogSynthView.h
 *
 *  Created on: 11 Jan 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../soundengine/asynth.h"
#include "../../framework/gui/binding/binding.h"

class AnalogSynthView : public ViewController {
private:
	AdvancedSynth& synth;
	std::array<size_t, ASYNTH_PART_COUNT> part_sizes = {1, 1, 1, 1, 1, 1, 1, 1};
	std::array<Button*, ASYNTH_PART_COUNT> operators;
	ViewHost* frame = nullptr;
	BindingGUIHandler binder;

	void position_operators();

public:
	AnalogSynthView(AdvancedSynth& s);
	virtual Scene create(ViewHost &frame);
	virtual void update_properties();

	virtual ~AnalogSynthView();
	virtual bool on_action(Control *control);
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_ */
