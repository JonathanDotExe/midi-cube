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


static inline double polyblep(double phase, double step) {
	if (phase < step) {
		phase /= step;
		return - phase * phase + 2 * phase - 1;
	}
	else if (phase > (1 - step)) {
		phase = (phase - 1)/step;
		return phase * phase + 2 * phase + 1;
	}
	return 0;
}

static inline double integrated_polyblep(double phase, double step) {
	if (phase < step) {
		phase = phase / step - 1;
		return - 1 / 3.0 * phase * phase * phase;
	}
	else if (phase > (1 - step)) {
		phase = (phase - 1) / step + 1;
		return 1 / 3.0 * phase * phase * phase;
	}
	return 0;
}

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
	double last_rotation = 0;
	double pulse_width = 0.5;
public:
	AnalogOscilator();
	void process(double freq, double time_step, AnalogOscilatorData& data);
	double carrier(double freq, double time_step, AnalogOscilatorData& data);
	double modulator(double freq, double time_step, AnalogOscilatorData& data);
	void randomize();
	void reset();
	~AnalogOscilator();
};

struct AnalogOscilatorBankData {
	double unison_detune = 0.1;
	size_t unison_amount = 0;
};

template<std::size_t U>
class UnisonOscilator {
private:
	std::array<AnalogOscilator, U> oscillators;
public:

	UnisonOscilator() {

	}

	AnalogOscilatorSignal signal(double freq, double time_step, AnalogOscilatorData data, AnalogOscilatorBankData bdata, bool carrier = true, bool modulator = true) {
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
			double f = freq * det;
			oscillators[i].process(f, time_step, data);
			if (carrier) {
				signal.carrier += oscillators[i].carrier(f, time_step, data);
			}
			if (modulator) {
				signal.modulator += oscillators[i].modulator(f, time_step, data);
			}
			det *= detune;
		}
		//Negative
		for (;  i < size; ++i) {
			double f = freq * ndet;
			oscillators[i].process(f, time_step, data);
			if (carrier) {
				signal.carrier += oscillators[i].carrier(f, time_step, data);
			}
			if (modulator) {
				signal.modulator += oscillators[i].modulator(f, time_step, data);
			}
			ndet *= ndetune;
		}
		signal.carrier /= size;
		signal.modulator /= size;
		return signal;
	}

	void randomize() {
		for (size_t i = 0; i < U; ++i) {
			oscillators[i].randomize();
		}
	}

	void reset() {
		for (size_t i = 0; i < U; ++i) {
			oscillators[i].reset();
		}
	}

	~UnisonOscilator() {

	}
};

#endif /* MIDICUBE_OSCILATOR_H_ */
