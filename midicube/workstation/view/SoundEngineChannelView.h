/*
 * SoundEngineChannelView.h
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_
#define MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../../framework/gui/binding/binding.h"
#include "../midicube.h"

class SoundEngineChannelView: public ViewController {
private:
	MidiCubeWorkstation& cube;
	SoundEngineChannel& channel;
	int channel_index;
	BindingGUIHandler binder;

public:
	SoundEngineChannelView(MidiCubeWorkstation& c, SoundEngineChannel& channel, int channel_index);
	virtual Scene create(ViewHost &frame);
	virtual ~SoundEngineChannelView();
	virtual bool on_action(Control *control);
};

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINECHANNELVIEW_H_ */
