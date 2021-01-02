/*
 * BOrganView.h
 *
 *  Created on: Dec 31, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_
#define MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../soundengine/organ.h"

class B3OrganView : public ViewController {
private:
	B3Organ& organ;
	SoundEngineChannel& channel;
	int channel_index;
	bool edit_midi = false;
public:
	B3OrganView(B3Organ& organ, SoundEngineChannel& channel, int channel_index);
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
	virtual ~B3OrganView();
};

#endif /* MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_ */
