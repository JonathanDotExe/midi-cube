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
protected:
	std::vector<std::string> properties;
public:
	Oscilator();
	virtual double signal(double time, double freq) = 0;
	virtual ~Oscilator();

	virtual void set_property(std::string name, double value) {

	}

	std::vector<std::string> get_properties() {
		return properties;
	}
};

class AnalogOscilator : public Oscilator {
private:
	AnalogWaveForm waveform;
public:
	AnalogOscilator(AnalogWaveForm waveform);
	double signal(double time, double freq);
	AnalogWaveForm get_waveform() const;
	void set_waveform(AnalogWaveForm waveform);
	~AnalogOscilator();
};

struct AdditiveSine {
	double harmonic;
	double amp;
};

class AdditiveOscilator : public Oscilator{
private:
	std::vector<AdditiveSine> sines;
public:
	AdditiveOscilator();
	double signal(double time, double freq);
	void add_sine(AdditiveSine sine);
	~AdditiveOscilator();
};

class SyncOscilator : public Oscilator{
private:
	double detune;
	AnalogWaveForm waveform;
public:
	SyncOscilator(AnalogWaveForm waveform, double detune);
	double signal(double time, double freq);
	void set_property(std::string name, double value);
	double get_detune() const;
	void set_detune(double detune);
	AnalogWaveForm get_waveform() const;
	void set_waveform(AnalogWaveForm waveform);
	~SyncOscilator();
};

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
	double signal(double time, double freq);
	unsigned int get_unison() const;
	void set_unison(unsigned int unison = 0);
	double get_unison_detune() const;
	void set_unison_detune(double unison_detune = 0.1);
	double get_volume() const;
	void set_volume(double volume = 1);
	std::vector<std::string> get_properties();
	void set_property(std::string name, double value);
	~OscilatorSlot();
};

#endif /* MIDICUBE_OSCILATOR_H_ */
