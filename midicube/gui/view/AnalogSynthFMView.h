/*
 * AnalogSynthFMView.h
 *
 *  Created on: Feb 9, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHFMVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHFMVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../soundengine/asynth.h"
#include "resources.h"

class AnalogSynthFMView : public ViewController{
private:
	AnalogSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
public:
	AnalogSynthFMView(AnalogSynth& s, SoundEngineChannel& c, int channel_index);
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
	virtual ~AnalogSynthFMView();
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHFMVIEW_H_ */
