/*
 * util.h
 *
 *  Created on: 28 Jun 2020
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
	T at(size_t pos);
	std::size_t size();
	std::size_t buffer_size();
};


#endif /* MIDICUBE_UTIL_H_ */
