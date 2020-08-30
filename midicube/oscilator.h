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
#include <vector>

enum class AnalogWaveForm {
	SINE, SAW, SQUARE, NOISE
};

class Oscilator {
public:
	Oscilator();
	virtual double signal(double time, double freq, double time_step) = 0;
	virtual ~Oscilator();
};

class AnalogOscilator : public Oscilator {
private:
	AnalogWaveForm waveform;
public:
	bool analog = false;
	double pulse_width = 0.5;
	AnalogOscilator(AnalogWaveForm waveform);
	double signal(double time, double freq, double time_step);
	AnalogWaveForm get_waveform() const;
	void set_waveform(AnalogWaveForm waveform);
	~AnalogOscilator();
};

//Deprecated
class OscilatorSlot {
private:
	Oscilator* osc;
	double volume = 1;
	unsigned int unison = 0;
	double unison_detune = 0.1;
	double udetune;
	double nudetune;

public:
	OscilatorSlot(Oscilator* osc);
	double signal(double time, double freq, double time_step);
	unsigned int get_unison() const;
	void set_unison(unsigned int unison = 0);
	double get_unison_detune() const;
	void set_unison_detune(double unison_detune = 0.1);
	double get_volume() const;
	void set_volume(double volume = 1);
	~OscilatorSlot();
};

#endif /* MIDICUBE_OSCILATOR_H_ */
