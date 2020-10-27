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
#include "synthesizer.h"
#include "sfsynth.h"

extern void fill_sound_engine_device(SoundEngineDevice* device) {
	device->add_sound_engine(new TemplateSoundEngineBank<PresetSynth>());
	device->add_sound_engine(new TemplateSoundEngineBank<B3Organ>());
	device->add_sound_engine(new TemplateSoundEngineBank<SampleDrums>());
	device->add_sound_engine(new TemplateSoundEngineBank<Sampler>());
	device->add_sound_engine(new TemplateSoundEngineBank<Vocoder>());
	device->add_sound_engine(new TemplateSoundEngineBank<Synthesizer>());
	device->add_sound_engine(new TemplateSoundEngineBank<SoundFontSynth>());
}



#endif /* MIDICUBE_SOUNDENGINE_ENGINES_H_ */
