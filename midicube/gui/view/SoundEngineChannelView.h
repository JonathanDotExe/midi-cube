/*
 * SoundEngineChannelView.h
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_
#define MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"

class SoundEngineChannelView: public ViewController {
private:
	SoundEngineChannel& channel;
	int channel_index;

public:
	SoundEngineChannelView(SoundEngineChannel& channel, int channel_index);
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
	virtual ~SoundEngineChannelView();
};

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_ */
