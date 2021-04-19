/*
 * EffectView.h
 *
 *  Created on: Apr 9, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_EFFECT_EFFECTVIEW_H_
#define MIDICUBE_GUI_VIEW_EFFECT_EFFECTVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "../../effect/effect.h"
#include "../../soundengine/soundengine.h"

class EffectView : public ViewController {
private:
	Effect* effect = nullptr;
	std::function<ViewController* ()> back;

public:
	EffectView(Effect* effect, std::function<ViewController* ()> b);
	virtual Scene create(Frame &frame);
	virtual ~EffectView();
};

#endif /* MIDICUBE_GUI_VIEW_EFFECT_EFFECTVIEW_H_ */
