/*
 * SourceView.h
 *
 *  Created on: Jun 18, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOURCEVIEW_H_
#define MIDICUBE_GUI_VIEW_SOURCEVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"

class SourceView : public ViewController {
public:
	SourceView();
	virtual ~SourceView();
	virtual Scene create(Frame &frame);
	virtual void update_properties();
};

#endif /* MIDICUBE_GUI_VIEW_SOURCEVIEW_H_ */
