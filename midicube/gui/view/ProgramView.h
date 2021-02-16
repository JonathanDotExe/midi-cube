/*
 * ProgramView.h
 *
 *  Created on: Feb 14, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_PROGRAMVIEW_H_
#define MIDICUBE_GUI_VIEW_PROGRAMVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"

class ProgramView : public ViewController {
private:
	SoundEngineDevice* engine = nullptr;
	ProgramManager* prog_mgr = nullptr;
	size_t page = 0;
public:
	ProgramView(size_t page = 0);
	virtual ~ProgramView();
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMVIEW_H_ */
