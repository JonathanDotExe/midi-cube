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
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"
#include "../view/resources.h"

class SourceView : public ViewController {
public:
	SourceView();
	virtual ~SourceView();
	virtual Scene create(Frame &frame);
	virtual void update_properties();
};

#endif /* MIDICUBE_GUI_VIEW_SOURCEVIEW_H_ */
