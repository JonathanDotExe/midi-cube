/*
 * wavetable.h
 *
 *  Created on: 18 Mar 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_WAVETABLE_H_
#define MIDICUBE_WAVETABLE_H_

#include <cstddef>
#include <cmath>
#include <array>

template <typename F, size_t N>
class WaveTable {
private:
	std::array<double, N> data;

public:

	void init() {
		for (size_t i = 0; i < N; ++i) {
			data[i] = F((double) i/(N - 1));
		}
	}

	inline double get_value(double prog) {
		double i = prog * (N - 1);
		size_t index = fmin(i, N - 2);
		double val1 = data[index];
		double val2 = data[index + 1];
		double p = i - index;
		return val1 * (1 - p) + val2 * p;
	}

};



#endif /* MIDICUBE_WAVETABLE_H_ */
