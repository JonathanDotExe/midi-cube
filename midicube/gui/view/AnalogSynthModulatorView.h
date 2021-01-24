/*
 * AnalogSynthModultatorView.h
 *
 *  Created on: Jan 24, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHMODULATORVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHMODULATORVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../soundengine/asynth.h"

class AnalogSynthModulatorView : public ViewController{
private:
	AnalogSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
	size_t part;
	bool edit_source = false;
public:
	AnalogSynthModulatorView(AnalogSynth& s, SoundEngineChannel& c, int channel_index, size_t part);
	virtual ~AnalogSynthModulatorView();
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
};

void property_mod_controls(std::vector<Control*>* controls, int x, int y, PropertyHolder* holder, size_t prop, std::string name, std::vector<Control*>* show_amount, std::vector<Control*>* show_source);

void adsr_controls(std::vector<Control*>* controls, int x, int y, PropertyHolder* holder, size_t prop);


#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHMODULATORVIEW_H_ */
