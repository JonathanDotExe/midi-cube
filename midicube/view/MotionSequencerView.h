/*
 * MotionSequencerView.h
 *
 *  Created on: Jun 14, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_MOTIONSEQUENCERVIEW_H_
#define MIDICUBE_GUI_VIEW_MOTIONSEQUENCERVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"
#include "../view/resources.h"

class MotionSequencerView : public ViewController {

public:
	MotionSequencerView();
	virtual Scene create(Frame &frame);
	virtual ~MotionSequencerView();
};

#endif /* MIDICUBE_GUI_VIEW_MOTIONSEQUENCERVIEW_H_ */
