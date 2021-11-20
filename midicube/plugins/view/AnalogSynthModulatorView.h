/*
 * AnalogSynthModultatorView.h
 *
 *  Created on: Jan 24, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHMODULATORVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHMODULATORVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../soundengine/asynth.h"
#include "../../framework/gui/binding/binding.h"

class AnalogSynthModulatorView : public ViewController{
private:
	AdvancedSynth& synth;
	size_t part;
	bool edit_source = false;
	BindingGUIHandler binder;
public:
	AnalogSynthModulatorView(AdvancedSynth& s, size_t part);
	virtual ~AnalogSynthModulatorView();
	virtual Scene create(ViewHost &frame);
	virtual bool on_action(Control *control);
	virtual ControlView* create_control_view();
};


#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHMODULATORVIEW_H_ */
