/*
 * ProgramRenameView.h
 *
 *  Created on: 18 Feb 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_
#define MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../midicube.h"
#include "../soundengine/soundengine.h"
#include <functional>
#include "../view/resources.h"

class ProgramRenameView : public ViewController {
private:
	std::string name;
	std::vector<char> chars;
	std::function<void(std::string name)> action = nullptr;
	MidiCube& cube;
public:
	ProgramRenameView(MidiCube& cube, std::string name, std::function<void(std::string name)> action);
	virtual Scene create(ViewHost &frame);
	virtual ~ProgramRenameView();
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_ */
