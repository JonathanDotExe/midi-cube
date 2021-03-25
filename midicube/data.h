/*
 * data.h
 *
 *  Created on: Mar 25, 2021
 *      Author: jojo
 */

#include <functional>


template <typename T>
class PropertyBinding {
private:
	std::function<T ()> get_func = nullptr;
	std::function<void (T)> set_func = nullptr;

public:
	T get() {
		return get_func();
	}

	void set(T t) {
		set_func(t);
	}

	bool is_bound() {
		return set_func && get_func;
	}

	template <typename E>
	void bind(E& e) {
		get_func = [&e]() {
			return e;
		};
		set_func = [&e](T t) {
			e = t;
		};
	}

};

class Action {

public:

	virtual void execute() = 0;

	virtual void returned() = 0;

	virtual ~Action() {

	}
};

template <typename T, typename V>
class GetValueAction {

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
class SetValueAction {

private:
	T& t;
	V v;

public:

	SetValueAction(T& var, V val) : t(var), V(val) {

	}

	virtual void execute() {
		t = v;
	}

	virtual void returned() {

	}

	virtual ~SetValueAction() {

	}
};



