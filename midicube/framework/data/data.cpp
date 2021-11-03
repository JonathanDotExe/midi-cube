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
	if (!realtime_actions.push(action)) {
		delete action;
		std::cout << "Warning: Lost requested action" << std::endl;
	}
}

void ActionHandler::execute_realtime_actions() {
	Action* action = nullptr;
	while (realtime_actions.pop(action)) {
		action->execute();
		if (!return_actions.push(action)) {
			delete action;
			std::cout << "Warning: Lost returned action" << std::endl;
		}
	}
}

bool ActionHandler::remaining_realtime_actions() {
	return !realtime_actions.empty();
}

bool ActionHandler::remaining_return_actions() {
	return !return_actions.empty();
}

void ActionHandler::execute_return_actions() {
	Action* action = nullptr;
	while (return_actions.pop(action)) {
		action->returned();
		delete action;
	}
}

FunctionAction::FunctionAction(std::function<void()> e,
		std::function<void()> r) : exec(e), ret(r) {
}

void FunctionAction::execute() {
	if (exec) {
		exec();
	}
}

void FunctionAction::returned() {
	if (ret) {
		ret();
	}
}
