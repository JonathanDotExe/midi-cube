/*
 * data.h
 *
 *  Created on: Mar 26, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_DATA_H_
#define MIDICUBE_DATA_H_


#include <functional>
#include "boost/lockfree/spsc_queue.hpp"


class Action {

public:

	virtual void execute() = 0;

	virtual void returned() = 0;

	virtual ~Action() {

	}
};

template <typename T, typename V>
class GetValueAction : public Action{

private:
	T& t;
	V v;
	std::function<void(V)> callback;

public:

	GetValueAction(T& var, std::function<void(V)> c) : t(var), callback(c) {

	}

	virtual void execute() {
		v = t;
	}

	virtual void returned() {
		callback(v);
	}

	virtual ~GetValueAction() {

	}
};

template <typename T, typename V>
class SetValueAction : public Action {

private:
	T& t;
	V v;

public:

	SetValueAction(T& var, V val) : t(var), v(val) {

	}

	virtual void execute() {
		t = static_cast<T>(v);
	}

	virtual void returned() {

	}

	virtual ~SetValueAction() {

	}
};

class ActionHandler {
private:
	boost::lockfree::spsc_queue<Action*> realtime_actions;
	boost::lockfree::spsc_queue<Action*> return_actions;
public:
	ActionHandler();
	void queue_action(Action* action);
	void execute_realtime_actions();
	void execute_return_actions();
};



template <typename T>
class PropertyBinding {
private:
	std::function<void (std::function<void (T)>)> get_func = nullptr;
	std::function<void (T)> set_func = nullptr;

public:
	void get(std::function<void (T)> callback) {
		get_func(callback);
	}

	void set(T t) {
		set_func(t);
	}

	bool is_bound() {
		return set_func && get_func;
	}

	template <typename E>
	void bind(E& e, ActionHandler& handler) {
		get_func = [&e, &handler](std::function<void (T)> callback) {
			handler.queue_action(new GetValueAction<E, T>(e, callback));
		};
		set_func = [&e, &handler](T t) {
			handler.queue_action(new SetValueAction<E, T>(e, t));
		};
	}

};

#endif /* MIDICUBE_DATA_H_ */
