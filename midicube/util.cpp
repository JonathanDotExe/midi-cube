/*
 * util.cpp
 *
 *  Created on: 28 Jun 2020
 *      Author: jojo
 */
#include "util.h"


template<class T, std::size_t N> T CircularBuffer<T, N>::at(size_t pos) {
	return buffer[(start + pos) %buffer.size()];
};

template<class T, std::size_t N> void CircularBuffer<T, N>::push_back(T t) {
	buffer[end] = t;
	++end;
	end %= buffer.size();
	if (end == start) {
		start++;
	}
};

template<class T, std::size_t N> std::size_t  CircularBuffer<T, N>::size() {
	return end < start ? (buffer.size() - start + end) : (end - start);
};

template<class T, std::size_t N> std::size_t  CircularBuffer<T, N>::buffer_size() {
	return buffer.size();
};

