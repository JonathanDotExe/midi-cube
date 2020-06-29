/*
 * util.h
 *
 *  Created on: 29 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_UTIL_H_
#define MIDICUBE_UTIL_H_

#include <array>


template<class T, std::size_t N> class CircularBuffer {
private:
	std::array<T, N> buffer;
	std::size_t start;
	std::size_t end;

public:
	T at(size_t pos) {
		return buffer[(start + pos) %buffer.size()];
	};

	T peek_first() {
		if (size() > 0) {
			return buffer[start];
		}
		throw std::runtime_error("Buffer is empty!");
	}

	void push_back(T t) {
		buffer[end] = t;
		++end;
		end %= buffer.size();
		if (end == start) {
			start++;
		}
	};

	T pop_first() {
		if (size() > 0) {
			T t = buffer[start++];
			start %= buffer.size();
			return t;
		}
		throw std::runtime_error("Buffer is empty!");
	}

	std::size_t size(){
		return end < start ? (buffer.size() - start + end) : (end - start);
	};

	std::size_t buffer_size() {
		return buffer.size();
	};
};


#endif /* MIDICUBE_UTIL_H_ */
