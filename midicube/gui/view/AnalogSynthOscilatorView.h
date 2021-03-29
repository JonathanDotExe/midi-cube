/*
 * AnalogSynthOscilatorView.h
 *
 *  Created on: Jan 20, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../soundengine/asynth.h"

class AnalogSynthOscilatorView: public ViewController {
private:
	AnalogSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
	size_t part;
	bool edit_source = false;
public:
	AnalogSynthOscilatorView(AnalogSynth& s, SoundEngineChannel& c, int channel_index, size_t part);
	virtual ~AnalogSynthOscilatorView();
	virtual Scene create(Frame &frame);
};

std::vector<DragBox<double>*> property_mod_controls(std::vector<Control*>* controls, int x, int y, PropertyModulation& mod, ActionHandler& handler, std::string name, std::vector<Control*>* show_amount, std::vector<Control*>* show_source);

void adsr_controls(std::vector<Control*>* controls, int x, int y, ADSREnvelopeData& data, ActionHandler& handler);

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_ */
