/*
 * MasterEffectView.h
 *
 *  Created on: Apr 11, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_MASTEREFFECTVIEW_H_
#define MIDICUBE_GUI_VIEW_MASTEREFFECTVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../../resources.h"
#include "../midicube.h"

class MasterEffectView : public ViewController {
private:
	MidiCube& cube;
public:
	MasterEffectView(MidiCube& cube);
	virtual ~MasterEffectView();
	virtual Scene create(ViewHost &frame);
};

#endif /* MIDICUBE_GUI_VIEW_MASTEREFFECTVIEW_H_ */
