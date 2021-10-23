/*
 * PluginView.h
 *
 *  Created on: Oct 23, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_WORKSTATION_VIEW_PLUGINVIEW_H_
#define MIDICUBE_WORKSTATION_VIEW_PLUGINVIEW_H_

#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../../framework/core/plugin.h"
#include "../../resources.h"

class PluginView: public ViewController {
private:
	PluginInstance& plugin;
	std::function<ViewController* ()> back;
public:
	PluginView(PluginInstance& plugin, std::function<ViewController* ()> back);
	virtual ~PluginView();
	virtual Scene create(ViewHost &frame);
};

#endif /* MIDICUBE_WORKSTATION_VIEW_PLUGINVIEW_H_ */
