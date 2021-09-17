/*
 * BOrganView.h
 *
 *  Created on: Dec 31, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_
#define MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../../soundengine/organ.h"
#include "../view/binding.h"

class B3OrganView : public ViewController {
private:
	B3Organ& organ;
	SoundEngineChannel& channel;
	int channel_index;
	bool edit_midi = false;
	BindingGUIHandler binder;
public:
	B3OrganView(B3Organ& organ, SoundEngineChannel& channel, int channel_index);
	virtual Scene create(Frame &frame);
	virtual ~B3OrganView();
	virtual bool on_action(Control *control);
};

#endif /* MIDICUBE_GUI_VIEW_B3ORGANVIEW_H_ */
