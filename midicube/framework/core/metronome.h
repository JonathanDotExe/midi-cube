/*
 * metronome.h
 *
 *  Created on: 10 Aug 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_METRONOME_H_
#define MIDICUBE_METRONOME_H_

#include <atomic>


class Metronome {

private:
	std::atomic<unsigned int> bpm;
	std::atomic<unsigned int> start_time;

public:
	Metronome(int bpm =120);

	void init (unsigned int time);

	bool is_beat(unsigned int time, unsigned int sample_rate, double value) const;

	double last_beat(unsigned int time, unsigned int sample_rate, double value) const;

	void set_bpm(unsigned int bpm);

	double beats(unsigned int time, unsigned int sample_rate, double value) const;

	unsigned int get_bpm() const;

};


#endif /* MIDICUBE_METRONOME_H_ */
