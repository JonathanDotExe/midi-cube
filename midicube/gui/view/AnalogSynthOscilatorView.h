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
public:
	AnalogSynthOscilatorView(AnalogSynth& s, SoundEngineChannel& c, int channel_index, size_t part);
	virtual ~AnalogSynthOscilatorView();
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHOSCILATORVIEW_H_ */
