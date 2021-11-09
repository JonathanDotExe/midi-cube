/*
 * plugin.cpp
 *
 *  Created on: 16 Sep 2021
 *      Author: jojo
 */
#include "plugin.h"


void PluginInstance::copy_plugin(Clipboard &clipboard) {
	PluginProgram* prog = nullptr;
	save_program(&prog);
	if (prog) {
		clipboard.copy(prog);
	}
}

bool PluginInstance::paste_plugin(Clipboard &clipboard) {
	PluginProgram* prog = clipboard.paste<PluginProgram>();
	bool success = false;
	if (prog) {
		apply_program(prog);
		success = true;
	}
	return success;
}
