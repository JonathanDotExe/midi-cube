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
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"

class SoundEngineView : public ViewController {
private:
	std::array<Button*, SOUND_ENGINE_MIDI_CHANNELS> engine_buttons;
	std::vector<std::string> engine_names;
	SoundEngineDevice* engine = nullptr;

public:
	SoundEngineView();

	virtual Scene create(Frame& frame);

	virtual void property_change(PropertyChange change);

	virtual ~SoundEngineView();

};

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_ */
