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
public:
	ProgramView();
	virtual ~ProgramView();
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMVIEW_H_ */
