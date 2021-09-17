/*
 * MasterEffectView.h
 *
 *  Created on: Apr 11, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_MASTEREFFECTVIEW_H_
#define MIDICUBE_GUI_VIEW_MASTEREFFECTVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../view/resources.h"

class MasterEffectView : public ViewController {
public:
	MasterEffectView();
	virtual ~MasterEffectView();
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_MASTEREFFECTVIEW_H_ */
