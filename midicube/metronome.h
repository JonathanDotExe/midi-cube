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

	bool is_beat(unsigned int time, unsigned int sample_rate, double value);

	double last_beat(unsigned int time, unsigned int sample_rate, double value);

	void set_bpm(unsigned int bpm);

	unsigned int get_bpm();

};


#endif /* MIDICUBE_METRONOME_H_ */
