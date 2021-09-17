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
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"
#include <functional>
#include "../view/resources.h"

class ProgramRenameView : public ViewController {
private:
	std::string name;
	std::vector<char> chars;
	std::function<void(std::string name)> action = nullptr;
public:
	ProgramRenameView(std::string name, std::function<void(std::string name)> action);
	virtual Scene create(Frame &frame);
	virtual ~ProgramRenameView();
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMRENAMEVIEW_H_ */
