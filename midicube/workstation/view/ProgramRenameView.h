/*
 * ProgramRenameView.h
 *
 *  Created on: 18 Feb 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_
#define MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../soundengine.h"
#include <functional>
#include "../../plugins/resources.h"
#include "../midicube.h"

class ProgramRenameView : public ViewController {
private:
	std::string name;
	std::vector<char> chars;
	std::function<void(std::string name)> action = nullptr;
	MidiCubeWorkstation& cube;
public:
	ProgramRenameView(MidiCubeWorkstation& cube, std::string name, std::function<void(std::string name)> action);
	virtual Scene create(ViewHost &frame);
	virtual ~ProgramRenameView();
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_ */
