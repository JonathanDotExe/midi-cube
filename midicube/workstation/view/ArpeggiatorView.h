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
#include "../sequencer/arpeggiator.h"

class ArpeggiatorView: public ViewController {
private:
	ArpeggiatorInstance& arp;

public:
	ArpeggiatorView(ArpeggiatorInstance& arp);
	virtual Scene create(ViewHost &frame);
	virtual ~ArpeggiatorView();
};

#endif /* MIDICUBE_GUI_VIEW_ARPEGGIATORVIEW_H_ */
