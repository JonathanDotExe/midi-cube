/*
 * EffectView.h
 *
 *  Created on: Apr 9, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_EFFECT_EFFECTVIEW_H_
#define MIDICUBE_GUI_VIEW_EFFECT_EFFECTVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../../framework/core/plugin.h"
#include "../../framework/gui/binding/binding.h"

class EffectView : public ViewController {
private:
	PluginInstance* effect = nullptr;
	BindingGUIHandler binder;
	bool edit_midi = false;

public:
	EffectView(PluginInstance* effect);
	virtual Scene create(ViewHost &frame);

	virtual ~EffectView();
	virtual bool on_action(Control *control);
};

#endif /* MIDICUBE_GUI_VIEW_EFFECT_EFFECTVIEW_H_ */
