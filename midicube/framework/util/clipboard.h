/*
 * clipboard.h
 *
 *  Created on: 8 Nov 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_UTIL_CLIPBOARD_H_
#define MIDICUBE_FRAMEWORK_UTIL_CLIPBOARD_H_

struct Copyable {

public:
	Copyable() {

	}

	virtual ~Copyable() {

	}

};


class Clipboard {
private:
	Copyable* value = nullptr;

public:
	void copy(Copyable* v) {
		if (this->value) {
			delete value;
		}
		this->value = v;
	}

	template<typename T>
	T* paste() {
		return dynamic_cast<T*>(value);
	}

	~Clipboard() {
		delete value;
	}

};


#endif /* MIDICUBE_FRAMEWORK_UTIL_CLIPBOARD_H_ */
