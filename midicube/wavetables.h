/*
 * wavetables.h
 *
 *  Created on: 18 Mar 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_WAVETABLES_H_
#define MIDICUBE_WAVETABLES_H_

#include "wavetable.h"

double analog_adsr_wave(double prog) {
	return 0;
}

WaveTable<1024> ANALOG_ADSR_WAVE(analog_adsr_wave);

void init_wavetables() {
	ANALOG_ADSR_WAVE.init();
}


#endif /* MIDICUBE_WAVETABLES_H_ */
