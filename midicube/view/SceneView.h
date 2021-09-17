/*
 * SceneView.h
 *
 *  Created on: Feb 28, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SCENEVIEW_H_
#define MIDICUBE_GUI_VIEW_SCENEVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"
#include "../view/resources.h"

class SceneView : public ViewController {
private:
	SoundEngineDevice* engine = nullptr;
	ActionHandler* handler = nullptr;
	std::array<Button*, SOUND_ENGINE_SCENE_AMOUNT> scenes;
public:
	SceneView();
	virtual ~SceneView();
	virtual Scene create(Frame &frame);
	virtual void update_properties();
};

#endif /* MIDICUBE_GUI_VIEW_SCENEVIEW_H_ */
