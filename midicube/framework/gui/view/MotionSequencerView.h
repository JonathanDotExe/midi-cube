/*
 * MotionSequencerView.h
 *
 *  Created on: Jun 14, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_MOTIONSEQUENCERVIEW_H_
#define MIDICUBE_GUI_VIEW_MOTIONSEQUENCERVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"

class MotionSequencerView : public ViewController {

public:
	MotionSequencerView();
	virtual Scene create(Frame &frame);
	virtual ~MotionSequencerView();
};

#endif /* MIDICUBE_GUI_VIEW_MOTIONSEQUENCERVIEW_H_ */
