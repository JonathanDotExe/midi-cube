/*
 * data.cpp
 *
 *  Created on: Mar 25, 2021
 *      Author: jojo
 */

#include "data.h"

ActionHandler::ActionHandler() : realtime_actions(2048), return_actions(2048) {
}

void ActionHandler::queue_action(Action *action) {
	realtime_actions.push(action);
}

void ActionHandler::execute_realtime_actions() {
	Action* action = nullptr;
	while (realtime_actions.pop(action)) {
		action->execute();
		return_actions.push(action);
	}
}

void ActionHandler::execute_return_actions() {
	Action* action = nullptr;
	while (return_actions.pop(action)) {
		action->execute();
		delete action;
	}
}


