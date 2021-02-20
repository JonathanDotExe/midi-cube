/*
 * ProgramRenameView.h
 *
 *  Created on: 18 Feb 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_
#define MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"
#include <functional>

class ProgramRenameView : public ViewController {
private:
	ProgramManager* prog_mgr = nullptr;
	std::vector<char> chars;
	std::function<void(ProgramManager* prog_mgr)> action = nullptr;
public:
	ProgramRenameView(std::function<void(ProgramManager* prog_mgr)> action);
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
	virtual ~ProgramRenameView();
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_ */
