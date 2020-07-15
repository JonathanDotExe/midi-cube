/*
 * synthesizer.h
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_
#define MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_

#include "soundengine.h"


class Synthesizer: public SoundEngine {
public:
	Synthesizer();
	virtual ~Synthesizer();
};

#endif /* MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_ */
