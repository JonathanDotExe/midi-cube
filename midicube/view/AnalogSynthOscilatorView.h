/*
 * AnalogSynthOscilatorView.h
 *
 *  Created on: Jan 20, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../soundengine/asynth.h"
#include "../view/binding.h"

class AnalogSynthOscilatorView: public ViewController {
private:
	AdvancedSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
	size_t part;
	bool edit_source = false;
	BindingGUIHandler binder;
public:
	AnalogSynthOscilatorView(AdvancedSynth& s, SoundEngineChannel& c, int channel_index, size_t part);
	virtual ~AnalogSynthOscilatorView();
	virtual Scene create(Frame &frame);
	virtual bool on_action(Control *control);
};

std::vector<DragBox<double>*> property_mod_controls(std::vector<Control*>* controls, int x, int y, PropertyModulation& mod, ActionHandler& handler, std::string name, std::vector<Control*>* show_amount, std::vector<Control*>* show_source);

void adsr_controls(std::vector<Control*>* controls, int x, int y, BindableADSREnvelopeData& data, ActionHandler& handler);

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_ */
