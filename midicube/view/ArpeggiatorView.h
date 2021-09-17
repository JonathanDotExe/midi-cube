/*
 * ArpeggiatorView.h
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_ARPEGGIATORVIEW_H_
#define MIDICUBE_GUI_VIEW_ARPEGGIATORVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"

class ArpeggiatorView: public ViewController {
private:
	SoundEngineChannel& channel;
	int channel_index;

public:
	ArpeggiatorView(SoundEngineChannel& channel, int channel_index);
	virtual Scene create(Frame &frame);
	virtual ~ArpeggiatorView();
};

#endif /* MIDICUBE_GUI_VIEW_ARPEGGIATORVIEW_H_ */
