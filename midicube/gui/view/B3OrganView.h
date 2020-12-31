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

class B3OrganView : public ViewController {
private:
	SoundEngineChannel& channel;
	int channel_index;
public:
	B3OrganView(SoundEngineChannel& channel, int channel_index);
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
	virtual ~B3OrganView();
};

#endif /* MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_ */
