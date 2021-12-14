/*
 * SourceView.h
 *
 *  Created on: Jun 18, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOURCEVIEW_H_
#define MIDICUBE_GUI_VIEW_SOURCEVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../midicube.h"
#include "../soundengine.h"
#include "../../plugins/resources.h"

class SourceView : public ViewController {
private:
	MidiCubeWorkstation& cube;

public:
	SourceView(MidiCubeWorkstation& c);
	virtual ~SourceView();
	virtual Scene create(ViewHost &frame);
	virtual void update_properties();
};

#endif /* MIDICUBE_GUI_VIEW_SOURCEVIEW_H_ */
