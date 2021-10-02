/*
 * AnalogSynthOperatorView.h
 *
 *  Created on: Feb 23, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHOPERATORVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHOPERATORVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../soundengine/asynth.h"
#include "../view/binding.h"

class AnalogSynthOperatorView : public ViewController {
private:
	AdvancedSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
	size_t part;
	bool edit_source = false;
	BindingGUIHandler binder;
public:
	AnalogSynthOperatorView(AdvancedSynth& s, SoundEngineChannel& c, int channel_index, size_t part);
	virtual ~AnalogSynthOperatorView();
	virtual Scene create(Frame &frame);
	virtual bool on_action(Control *control);
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHOPERATORVIEW_H_ */
