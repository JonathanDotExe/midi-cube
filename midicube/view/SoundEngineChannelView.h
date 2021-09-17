/*
 * SoundEngineChannelView.h
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_
#define MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "binding.h"

class SoundEngineChannelView: public ViewController {
private:
	SoundEngineChannel& channel;
	int channel_index;
	BindingGUIHandler binder;

public:
	SoundEngineChannelView(SoundEngineChannel& channel, int channel_index);
	virtual Scene create(Frame &frame);
	virtual ~SoundEngineChannelView();
	virtual bool on_action(Control *control);
};

ViewController* create_view_for_engine(std::string name, SoundEngine& engine, SoundEngineChannel& channel, int channel_index);

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_ */
