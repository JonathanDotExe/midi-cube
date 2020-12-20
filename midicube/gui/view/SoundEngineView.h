/*
 * SoundEngineView.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_
#define MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"

class SoundEngineView : public ViewController {
public:
	SoundEngineView();

	virtual std::vector<Control*> create(Frame& frame);

	virtual ~SoundEngineView();
};

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_ */
