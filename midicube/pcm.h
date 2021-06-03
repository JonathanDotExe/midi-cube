/*
 * pcm.h
 *
 *  Created on: Jun 3, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_PCM_H_
#define MIDICUBE_PCM_H_

#include "audiofile.h"

struct PCMWaveFormZone {
	AudioSample sample;
	unsigned int max_note = 127;
	unsigned int loop_start = 0;
	unsigned int loop_end = 0;
};

struct PCMWaveForm {
	std::string name = "WaveForm";
	std::vector<PCMWaveFormZone> zones;
};



#endif /* MIDICUBE_PCM_H_ */
