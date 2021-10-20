/*
 * AnalogSynthFMView.h
 *
 *  Created on: Feb 9, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ANALOGSYNTHFMVIEW_H_
#define MIDICUBE_GUI_VIEW_ANALOGSYNTHFMVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../soundengine/asynth.h"
#include "../view/resources.h"

class AnalogSynthFMView : public ViewController{
private:
	AdvancedSynth& synth;
public:
	AnalogSynthFMView(AdvancedSynth& s);
	virtual Scene create(ViewHost &frame);
	virtual ~AnalogSynthFMView();
};

#endif /* MIDICUBE_GUI_VIEW_ANALOGSYNTHFMVIEW_H_ */
