/*
 * SceneView.h
 *
 *  Created on: Feb 28, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SCENEVIEW_H_
#define MIDICUBE_GUI_VIEW_SCENEVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../soundengine.h"
#include "../../plugins/resources.h"
#include "../midicube.h"

class SceneView : public ViewController {
private:
	SoundEngineDevice* engine = nullptr;
	std::array<Button*, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	ActionHandler* handler = nullptr;
public:
	SceneView(SoundEngineDevice* engine);
	virtual ~SceneView();
	virtual Scene create(ViewHost &frame);
	virtual void update_properties();
};

#endif /* MIDICUBE_GUI_VIEW_SCENEVIEW_H_ */
