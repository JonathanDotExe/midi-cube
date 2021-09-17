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

#define PROGRAM_VIEW_ROWS 4
#define PROGRAM_VIEW_COLS 8


class ProgramView : public ViewController {
private:
	size_t bank = 0;
	size_t page = 0;
public:
	ProgramView(size_t bank = 0, size_t page = 0);
	virtual ~ProgramView();
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_PROGRAMVIEW_H_ */
