/*
 * pcm.h
 *
 *  Created on: Jun 3, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_PCM_H_
#define MIDICUBE_PCM_H_

#include "../util/audiofile.h"

struct PCMWaveFormZone {
	AudioSample sample;
	double freq = 440;
	unsigned int max_note = 127;
	double loop_start = 0;
	double loop_end = 0;
};

struct PCMWaveForm {
	std::string name = "WaveForm";
	std::vector<PCMWaveFormZone> zones;
};

struct PCMOscilatorData {
	PCMWaveFormZone* zone = nullptr;
};

class PCMOscilator {
private:
	double time = 0;

public:
	PCMOscilator();
	double process(double freq, double time_step, PCMOscilatorData& data);
	void reset();
	~PCMOscilator();
};


#endif /* MIDICUBE_PCM_H_ */
