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
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"

class SceneView : public ViewController {
private:
	SoundEngineDevice* engine = nullptr;
	std::array<Button*, SOUND_ENGINE_SCENE_AMOUNT> scenes;
public:
	SceneView();
	virtual ~SceneView();
	virtual void property_change(PropertyChange change);
	virtual Scene create(Frame &frame);
};

#endif /* MIDICUBE_GUI_VIEW_SCENEVIEW_H_ */
