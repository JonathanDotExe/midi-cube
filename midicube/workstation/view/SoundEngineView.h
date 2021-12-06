/*
 * SoundEngineView.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_
#define MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../soundengine.h"
#include "../../plugins/resources.h"
#include "../midicube.h"

class SoundEngineView : public ViewController {
private:
	std::array<Button*, SOUND_ENGINE_MIDI_CHANNELS> engine_buttons;
	MidiCubeWorkstation& cube;
	SoundEngineDevice* engine = nullptr;

public:
	SoundEngineView(MidiCubeWorkstation& c);

	virtual Scene create(ViewHost& frame);

	virtual ~SoundEngineView();

};

#endif /* MIDICUBE_GUI_VIEW_SOUNDENGINEVIEW_H_ */
