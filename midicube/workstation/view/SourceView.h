/*
 * SourceView.h
 *
 *  Created on: Jun 18, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOURCEVIEW_H_
#define MIDICUBE_GUI_VIEW_SOURCEVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../midicube.h"
#include "../soundengine.h"
#include "../../resources.h"

class SourceView : public ViewController {
private:
	MidiCube& cube;

public:
	SourceView(MidiCube& c);
	virtual ~SourceView();
	virtual Scene create(ViewHost &frame);
	virtual void update_properties();
};

#endif /* MIDICUBE_GUI_VIEW_SOURCEVIEW_H_ */
