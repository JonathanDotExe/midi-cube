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

struct AnalogOscilatorSignal {
	double carrier = 0;
	double modulator = 0;
};

class AnalogOscilator {
private:
	double rotation = 0;
	double sync_rotation = 0;
	double pulse_width = 0.5;
	double last_phase = 0;
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
		std::array<AnalogOscilator, U>& osc = oscilators.at(index);
		AnalogOscilatorSignal signal;
		double detune = note_to_freq_transpose(bdata.unison_detune);
		double ndetune = note_to_freq_transpose(-bdata.unison_detune);
		double det = detune;
		double ndet = 1;
		for (size_t i = 0; i <= bdata.unison_amount && i < osc.size(); ++i) {
			double d = 1;
			if (i % 2 == 0) {
				d = ndet;
				ndet *= ndetune;
			}
			else {
				d = det;
				det *= detune;
			}
			AnalogOscilatorSignal sig = osc[i].signal(freq * d, time_step, data);
			signal.carrier += sig.carrier;
			signal.modulator += signal.modulator;
		}
		signal.carrier /= (bdata.unison_amount + 1);
		signal.modulator /= (bdata.unison_amount + 1);
		return signal;
	}

	void randomize(size_t index) {
		std::array<AnalogOscilator, U>& osc = oscilators.at(index);
		for (size_t i = 0; i < osc.size(); ++i) {
			osc[i].randomize();
		}
	}

	void reset(size_t index) {
		std::array<AnalogOscilator, U>& osc = oscilators.at(index);
		for (size_t i = 0; i < osc.size(); ++i) {
			osc[i].reset();
		}
	}

	~AnalogOscilatorBank() {

	}
};

#endif /* MIDICUBE_OSCILATOR_H_ */
