/*
 * metronome.h
 *
 *  Created on: 10 Aug 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_METRONOME_H_
#define MIDICUBE_METRONOME_H_



class Metronome {

private:
	unsigned int bpm;
	unsigned int start_time;

public:
	Metronome(int bpm);

	void init (unsigned int time);

	bool is_beat(unsigned int time, unsigned int sample_rate, int value);

	void set_bpm(unsigned int bpm);

	unsigned int get_bpm();

};


#endif /* MIDICUBE_METRONOME_H_ */
