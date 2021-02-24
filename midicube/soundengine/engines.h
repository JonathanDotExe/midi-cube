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
#include "sampler.h"
#include "sfsynth.h"
#include "asynth.h"

extern void fill_sound_engine_device(SoundEngineDevice* device) {
	device->add_sound_engine(new TemplateSoundEngineBank<Sampler>());
	device->add_sound_engine(new TemplateSoundEngineBank<B3Organ>());
	device->add_sound_engine(new TemplateSoundEngineBank<AnalogSynth>());
	device->add_sound_engine(new TemplateSoundEngineBank<SampleDrums>());
	device->add_sound_engine(new TemplateSoundEngineBank<SoundFontSynth>());
}



#endif /* MIDICUBE_SOUNDENGINE_ENGINES_H_ */
