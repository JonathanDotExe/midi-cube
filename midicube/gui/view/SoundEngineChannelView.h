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
	virtual Scene create(Frame &frame);
	virtual ~SoundEngineChannelView();
};

ViewController* create_view_for_engine(std::string name, SoundEngine& engine, SoundEngineChannel& channel, int channel_index);

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_ */
