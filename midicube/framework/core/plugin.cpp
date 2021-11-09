/*
 * plugin.cpp
 *
 *  Created on: 16 Sep 2021
 *      Author: jojo
 */
#include "plugin.h"


void PluginSlot::copy_plugin(Clipboard &clipboard) {
	PluginSlotProgram* prog = new PluginSlotProgram();
	save(*prog);
	clipboard.copy(prog);
}

bool PluginSlot::paste_plugin(Clipboard &clipboard, PluginManager& mgr) {
	PluginSlotProgram* prog = clipboard.paste<PluginSlotProgram>();
	bool success = false;
	if (prog) {
		load(*prog, &mgr);
		success = true;
	}
	return success;
}
