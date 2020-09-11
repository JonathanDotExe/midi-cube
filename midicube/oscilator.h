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
	SINE, SAW_DOWN, SAW_UP, SQUARE, NOISE
};

struct AnalogOscilatorData {
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN;
	bool analog = false;
	bool sync = false;
	double pulse_width = 0.5;
	double sync_mul = 1;
};

class AnalogOscilator {
private:
	double rotation = 0;
	double sync_rotation = 0;
	double pulse_width = 0.5;
	double last_phase = 0;
public:
	AnalogOscilator();
	double signal(double freq, double time_step, AnalogOscilatorData& data);
	~AnalogOscilator();
};

template<std::size_t N, std::size_t U>
class AnalogOscilatorBank {
private:
	std::array<std::array<AnalogOscilator, U>, N> oscilators;
public:
	AnalogOscilatorData data;
	double unison_detune = 0.1;
	size_t unison_amount = 0;

	AnalogOscilatorBank() {

	}

	double signal(double freq, double time_step, size_t index) {
		std::array<AnalogOscilator, U>& osc = oscilators.at(index);
		double signal = 0;
		double detune = note_to_freq_transpose(unison_detune);
		double ndetune = note_to_freq_transpose(-unison_detune);
		double det = detune;
		double ndet = 1;
		for (size_t i = 0; i <= unison_amount && i < osc.size(); ++i) {
			double d = 1;
			if (i % 2 == 0) {
				d = ndet;
				ndet *= ndetune;
			}
			else {
				d = det;
				det *= detune;
			}
			signal += osc[i].signal(freq * d, time_step, data);
		}
		return signal / (unison_amount + 1);
	}

	~AnalogOscilatorBank() {

	}
};

#endif /* MIDICUBE_OSCILATOR_H_ */
