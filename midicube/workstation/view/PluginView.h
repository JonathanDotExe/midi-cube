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
	ViewHost* host = nullptr;
	std::function<ViewController* ()> back;
	bool* play_metronome;
	Metronome* metronome;
	double* master_volume;
public:
	PluginView(PluginInstance& plugin, std::function<ViewController* ()> back, Metronome* metronome, bool* play_metronome, double* master_volume);
	virtual ~PluginView();
	virtual Scene create(ViewHost &frame);
	virtual ControlView* create_control_view();
};

#endif /* MIDICUBE_WORKSTATION_VIEW_PLUGINVIEW_H_ */
