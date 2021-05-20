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
#include "asynth.h"

#include "../effect/amplifier_simulation.h"
#include "../effect/bitcrusher.h"
#include "../effect/chorus.h"
#include "../effect/delay.h"
#include "../effect/flanger.h"
#include "../effect/phaser.h"
#include "../effect/reverb.h"
#include "../effect/rotary_speaker.h"
#include "../effect/tremolo.h"
#include "../effect/vocoder.h"
#include "../effect/wahwah.h"

extern void fill_sound_engine_device(SoundEngineDevice* device) {
	device->add_sound_engine(new TemplateSoundEngineBuilder<Sampler>());
	device->add_sound_engine(new TemplateSoundEngineBuilder<B3Organ>());
	device->add_sound_engine(new TemplateSoundEngineBuilder<AnalogSynth>());
	device->add_sound_engine(new TemplateSoundEngineBuilder<SampleDrums>());

	device->add_effect(new TemplateEffectBuilder<AmplifierSimulationEffect>());
	device->add_effect(new TemplateEffectBuilder<RotarySpeakerEffect>());
	device->add_effect(new TemplateEffectBuilder<ReverbEffect>());
	device->add_effect(new TemplateEffectBuilder<ChorusEffect>());
	device->add_effect(new TemplateEffectBuilder<BitCrusherEffect>());
	device->add_effect(new TemplateEffectBuilder<VocoderEffect>());
	device->add_effect(new TemplateEffectBuilder<TremoloEffect>());
	device->add_effect(new TemplateEffectBuilder<DelayEffect>());
	device->add_effect(new TemplateEffectBuilder<FlangerEffect>());
	device->add_effect(new TemplateEffectBuilder<PhaserEffect>());
	device->add_effect(new TemplateEffectBuilder<WahWahEffect>());
}



#endif /* MIDICUBE_SOUNDENGINE_ENGINES_H_ */
