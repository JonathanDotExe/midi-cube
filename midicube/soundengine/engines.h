/*
 * engines.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_ENGINES_H_
#define MIDICUBE_SOUNDENGINE_ENGINES_H_

#include "organ.h"
#include "drums.h"
#include "presetsynth.h"
#include "sampler.h"
#include "vocoder.h"

extern void fill_sound_engine_device(SoundEngineDevice* device) {
	device->add_sound_engine(new PresetSynth());
	device->add_sound_engine(new B3Organ());
	device->add_sound_engine(new SampleDrums());
	device->add_sound_engine(new Sampler());
	device->add_sound_engine(new Vocoder());
}



#endif /* MIDICUBE_SOUNDENGINE_ENGINES_H_ */
