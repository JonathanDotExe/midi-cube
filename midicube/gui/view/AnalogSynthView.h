/*
 * AnalogSynthView.h
 *
 *  Created on: 11 Jan 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../soundengine/asynth.h"

class AnalogSynthView: public ViewController {
private:
	AnalogSynth& synth;
	SoundEngineChannel& channel;
	int channel_index;
	size_t part;
public:
	AnalogSynthView(AnalogSynth& s, SoundEngineChannel& c, int channel_index);
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
	virtual ~AnalogSynthView();
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHVIEW_H_ */
