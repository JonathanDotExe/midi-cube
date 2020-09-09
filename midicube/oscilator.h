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
};

class AnalogOscilator {
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

//Deprecated
template<std::size_t N>
class OscilatorSlot {
private:
	double volume = 1;
	unsigned int unison = 0;
	double unison_detune = 0.1;
	double udetune;
	double nudetune;

public:
	AnalogOscilatorBank<N> osc;

public:
	OscilatorSlot() {
		set_unison_detune(0);
	}

	double signal(double time, double freq, double time_step, size_t index) {
		double signal = osc.signal(time, freq, time_step, index);
		double det = udetune;
		double ndet = nudetune;
		for (unsigned int i = 1; i <= unison; ++i) {
			if (i % 2 == 0) {
				signal += osc.signal(time, freq * ndet, time_step, index);
				ndet *= nudetune;
			}
			else {
				signal += osc.signal(time, freq * det, time_step, index);
				det *= udetune;
			}
		}
		return signal / (1 + unison) * volume;
	}

	unsigned int get_unison() const {
		return unison;
	}
	void set_unison(unsigned int unison = 0) {
		this->unison = unison;
	}
	double get_unison_detune() const {
		return unison_detune;
	}
	void set_unison_detune(double unison_detune = 0.1) {
		this->unison_detune = unison_detune;
		udetune = note_to_freq_transpose(unison_detune);
		nudetune = note_to_freq_transpose(-unison_detune);
	}
	double get_volume() const {
		return volume;
	}
	void set_volume(double volume = 1) {
		this->volume = volume;
	}
	~OscilatorSlot() {

	}
};

#endif /* MIDICUBE_OSCILATOR_H_ */
