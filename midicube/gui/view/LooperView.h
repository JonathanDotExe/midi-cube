/*
 * LooperView.h
 *
 *  Created on: Apr 14, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_LOOPERVIEW_H_
#define MIDICUBE_GUI_VIEW_LOOPERVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"

class LooperView : public ViewController {
private:
	std::array<Button*, LOOPER_CHANNELS> records;
	std::array<Button*, LOOPER_CHANNELS> solos;
public:
	LooperView();
	virtual ~LooperView();
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_LOOPERVIEW_H_ */
