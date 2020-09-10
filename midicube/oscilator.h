/*
 * oscilator.h
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_OSCILATOR_H_
#define MIDICUBE_OSCILATOR_H_

#include "envelope.h"
#include "audio.h"
#include "synthesis.h"
#include <vector>
#include <array>

enum class AnalogWaveForm {
	SINE, SAW, SQUARE, NOISE
};

struct AnalogOscilatorData {
	AnalogWaveForm waveform = AnalogWaveForm::SAW;
	bool analog = false;
	double pulse_width = 0.5;
	unsigned int unison_amount = 0;
	double unison_detune = 0.1;
	double sync = 1;
};

class AnalogOscilator {
private:
	double rotation = 0;
public:
	AnalogOscilator();
	double signal(double time, double freq, double time_step, AnalogOscilatorData& data);
	~AnalogOscilator();
};

template<std::size_t N>
class AnalogOscilatorBank {
private:
	std::array<AnalogOscilator, N> oscilators;
public:
	AnalogOscilatorData data;

	AnalogOscilatorBank() {

	}

	double signal(double time, double freq, double time_step, size_t index) {
		return oscilators.at(index).signal(time, freq, time_step, data);
	}

	~AnalogOscilatorBank() {

	}
};

#endif /* MIDICUBE_OSCILATOR_H_ */
