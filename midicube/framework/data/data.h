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
#include "boost/lockfree/queue.hpp"
#include "../util/util.h"
#include "binding.h"

template <typename T>
class PropertyBinding {
private:
	std::function<void (std::function<void (T)>)> get_func = nullptr;
	std::function<void (T)> set_func = nullptr;

	BindableValue* object = nullptr;

public:

	BindableValue* get_object() {
		return object;
	}

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
	void bind(E& e, SpinLock& lock) {
		get_func = [&e, &lock](std::function<void (T)> callback) {
			lock.lock();
			callback(e);
			lock.unlock();
		};
		set_func = [&e, &lock](T t) {
			lock.lock();
			e = t;
			lock.unlock();
		};

		if (std::is_base_of<BindableValue, E>()) {
			object = (BindableValue*) (&e); //FIXME
		}
	}

	template <typename E>
	void bind_cast(E& e, SpinLock& lock) {
		get_func = [&e, &lock](std::function<void (T)> callback) {
			lock.lock();
			callback(e);
			lock.unlock();
		};
		set_func = [&e, &lock](T t) {
			lock.lock();
			e = static_cast<E>(t);
			lock.unlock();
		};

		if (std::is_base_of<BindableValue, E>()) {
			object = (BindableValue*)(&e); //FIXME
		}
	}

	template <typename E>
	void bind_function(std::function<E ()> get, std::function<void (E)> set, SpinLock& lock) {
		get_func = [get, &lock](std::function<void (T)> callback) {
			lock.lock();
			callback(get());
			lock.unlock();
		};
		set_func = [set, &lock](T t) {
			lock.lock();
			set(t);
			lock.unlock();
		};
		object = nullptr;
	}

};

#endif /* MIDICUBE_DATA_H_ */
