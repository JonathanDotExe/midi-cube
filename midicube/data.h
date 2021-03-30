/*
 * data.h
 *
 *  Created on: Mar 26, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_DATA_H_
#define MIDICUBE_DATA_H_


#include <functional>
#include <iostream>
#include "boost/lockfree/spsc_queue.hpp"


class Action {

public:

	virtual void execute() = 0;

	virtual void returned() = 0;

	virtual ~Action() {

	}
};

class FunctionAction : public Action {

private:
	std::function<void()> exec;
	std::function<void()> ret;

public:

	FunctionAction(std::function<void()> e, std::function<void()> r);

	virtual void execute();

	virtual void returned();

	virtual ~FunctionAction() {

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
class GetFunctionAction : public Action{

private:
	std::function<T ()> get_func;
	V v;
	std::function<void(V)> callback;

public:

	GetFunctionAction(std::function<T ()> f, std::function<void(V)> c) : get_func(f), callback(c) {

	}

	virtual void execute() {
		v = get_func();
	}

	virtual void returned() {
		callback(v);
	}

	virtual ~GetFunctionAction() {

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

template <typename T, typename V>
class SetFunctionAction : public Action {

private:
	std::function<void (T)> set_func;
	V v;

public:

	SetFunctionAction(std::function<void (T)> f, V val) : set_func(f), v(val) {

	}

	virtual void execute() {
		set_func(static_cast<T>(v));
	}

	virtual void returned() {

	}

	virtual ~SetFunctionAction() {

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

	template <typename E>
	void bind_function(std::function<E ()> get, std::function<void (E)> set, ActionHandler& handler) {
		get_func = [get, &handler](std::function<void (T)> callback) {
			handler.queue_action(new GetFunctionAction<E, T>(get, callback));
		};
		set_func = [set, &handler](T t) {
			handler.queue_action(new SetFunctionAction<E, T>(set, t));
		};
	}

};

#endif /* MIDICUBE_DATA_H_ */
