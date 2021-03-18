/*
 * wavetables.h
 *
 *  Created on: 18 Mar 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_WAVETABLES_H_
#define MIDICUBE_WAVETABLES_H_

#include "wavetable.h"
#include <cmath>

double analog_adsr_wave(double prog) {
	return pow(prog, 2.0/3.0);
}

WaveTable<1024> ANALOG_ADSR_WAVE(analog_adsr_wave);



#endif /* MIDICUBE_WAVETABLES_H_ */
