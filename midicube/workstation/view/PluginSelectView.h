/*
 * PluginSelectView.h
 *
 *  Created on: 24 Oct 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_WORKSTATION_VIEW_PLUGINSELECTVIEW_H_
#define MIDICUBE_WORKSTATION_VIEW_PLUGINSELECTVIEW_H_

#include "../../framework/core/plugin.h"
#include "../../framework/gui/core.h"
#include "../../framework/gui/control.h"
#include "../../resources.h"

class PluginSelectView : public ViewController {
private:
	PluginSlot& plugin;
	std::vector<Plugin*> plugins;
	std::function<ViewController* ()> back;
	size_t page = 0;
	SpinLock& lock;
public:
	PluginSelectView(PluginSlot& plugin, std::vector<Plugin*> plugins, SpinLock& lock, std::function<ViewController* ()> back, size_t page = 0);
	virtual ~PluginSelectView();
	virtual Scene create(ViewHost &frame);
};

#endif /* MIDICUBE_WORKSTATION_VIEW_PLUGINSELECTVIEW_H_ */
