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

enum AnalogWaveForm {
	SINE_WAVE, SAW_DOWN_WAVE, SAW_UP_WAVE, SQUARE_WAVE, TRIANGLE_WAVE, NOISE_WAVE
};

struct AnalogOscilatorData {
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN_WAVE;
	bool analog = false;
	bool sync = false;
	double pulse_width = 0.5;
	double sync_mul = 1;
};

struct AnalogOscilatorSignal {
	double carrier = 0;
	double modulator = 0;
};

class AnalogOscilator {
private:
	double rotation = 0;
	double pulse_width = 0.5;
public:
	AnalogOscilator();
	AnalogOscilatorSignal signal(double freq, double time_step, AnalogOscilatorData& data);
	void randomize();
	void reset();
	~AnalogOscilator();
};

struct AnalogOscilatorBankData {
	double unison_detune = 0.1;
	size_t unison_amount = 0;
};

template<std::size_t N, std::size_t U>
class AnalogOscilatorBank {
private:
	std::array<std::array<AnalogOscilator, U>, N> oscilators;
public:

	AnalogOscilatorBank() {

	}

	AnalogOscilatorSignal signal(double freq, double time_step, size_t index, AnalogOscilatorData data, AnalogOscilatorBankData bdata) {
		std::array<AnalogOscilator, U>& osc = oscilators[index];
		AnalogOscilatorSignal signal;
		double detune = note_to_freq_transpose(bdata.unison_detune);
		double ndetune = note_to_freq_transpose(-bdata.unison_detune);
		double det = detune;
		double ndet = 1;

		const size_t size = std::min(bdata.unison_amount + 1, U);
		const size_t psize = size/2;
		//Positive
		size_t i = 0;
		for (;  i < psize; ++i) {
			AnalogOscilatorSignal sig = osc[i].signal(freq * det, time_step, data);
			signal.carrier += sig.carrier;
			signal.modulator += sig.modulator;
			det *= detune;
		}
		//Negative
		for (;  i < size; ++i) {
			AnalogOscilatorSignal sig = osc[i].signal(freq * ndet, time_step, data);
			signal.carrier += sig.carrier;
			signal.modulator += sig.modulator;
			ndet *= ndetune;
		}
		signal.carrier /= size;
		signal.modulator /= size;
		return signal;
	}

	void randomize(size_t index) {
		std::array<AnalogOscilator, U>& osc = oscilators[index];
		for (size_t i = 0; i < U; ++i) {
			osc[i].randomize();
		}
	}

	void reset(size_t index) {
		std::array<AnalogOscilator, U>& osc = oscilators[index];
		for (size_t i = 0; i < U; ++i) {
			osc[i].reset();
		}
	}

	~AnalogOscilatorBank() {

	}
};

#endif /* MIDICUBE_OSCILATOR_H_ */
