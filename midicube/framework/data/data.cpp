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
	if (closed || !realtime_actions.push(action)) {
		delete action;
		std::cout << "Warning: Lost requested action" << std::endl;
	}
	else if (closed) {
		Action* action = nullptr;
		while (realtime_actions.pop(action)) {
			delete action;
		}
	}
}

void ActionHandler::execute_realtime_actions() {
	Action* action = nullptr;
	while (realtime_actions.pop(action)) {
		action->execute();
		if (closed || !return_actions.push(action)) {
			delete action;
			std::cout << "Warning: Lost returned action" << std::endl;
		}
		else if (closed) {
			while (return_actions.pop(action)) {
				delete action;
			}
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

ActionHandler::~ActionHandler() {
	closed = true;
	Action* action = nullptr;
	while (realtime_actions.pop(action)) {
		delete action;
	}
	while (return_actions.pop(action)) {
		delete action;
	}
}

void ActionHandler::wait_till_finished_gui() {
	//Wait for tasks
	while (remaining_realtime_actions()) {
		std::this_thread::yield();
	}
	while (remaining_return_actions()) {
		execute_return_actions();
		std::this_thread::yield();
	}
}
