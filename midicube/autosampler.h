/*
 * autosampler.h
 *
 *  Created on: 1 Mar 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_AUTOSAMPLER_H_
#define MIDICUBE_AUTOSAMPLER_H_

#include <vector>
#include <array>
#include <string>

class AutoSampler {
public:
	unsigned int channel = 0;
	std::vector<unsigned int> notes;
	std::vector<unsigned int> velocities;

	inline int process(double* output_buffer, double* input_buffer, unsigned int buffer_size, double time);

};



#endif /* MIDICUBE_AUTOSAMPLER_H_ */
