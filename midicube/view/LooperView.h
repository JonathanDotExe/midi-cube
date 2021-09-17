/*
 * LooperView.h
 *
 *  Created on: Apr 14, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_LOOPERVIEW_H_
#define MIDICUBE_GUI_VIEW_LOOPERVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"

class LooperView : public ViewController {
private:
	std::array<Button*, LOOPER_CHANNELS> records;
	std::array<Button*, LOOPER_CHANNELS> solos;
	ssize_t record_index = -1;
	ssize_t solo_index = -1;
	ActionHandler* handler = nullptr;
	SoundEngineDevice* engine = nullptr;
	void update_record();
	void update_solo();
public:
	LooperView();
	virtual void update_properties();
	virtual ~LooperView();
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_LOOPERVIEW_H_ */
