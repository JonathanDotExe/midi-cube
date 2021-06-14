project('midi-cube', 'cpp', default_options: ['cpp_std=c++17'])
cxx = meson.get_compiler('cpp')

deps = [
	dependency('rtaudio'),
	dependency('rtmidi'),
	dependency('sndfile'),
	dependency('sfml-system'),
	dependency('sfml-window'),
	dependency('sfml-graphics'),
	cxx.find_library('boost_system'),
	cxx.find_library('boost_filesystem')
]

src = [
	'midicube-main.cpp',
	
	'midicube/audio.cpp',
	'midicube/audiofile.cpp',
	'midicube/binding.cpp',
	'midicube/data.cpp',
	'midicube/envelope.cpp',
	'midicube/filter.cpp',
	'midicube/looper.cpp',
	'midicube/metronome.cpp',
	'midicube/midi.cpp',
	'midicube/midicube.cpp',
	'midicube/program.cpp',
	'midicube/oscilator.cpp',
	'midicube/sfz.cpp',
	'midicube/synthesis.cpp',
	'midicube/util.cpp',
	'midicube/wavetable.cpp',
	
	'midicube/soundengine/asynth.cpp',
	'midicube/soundengine/drums.cpp',
	'midicube/soundengine/organ.cpp',
	'midicube/soundengine/sampler.cpp',
	'midicube/soundengine/soundengine.cpp',
	'midicube/soundengine/voice.cpp',
	
	'midicube/gui/engine/control.cpp',
	'midicube/gui/engine/core.cpp',
	'midicube/gui/engine/util.cpp',
	
	'midicube/gui/view/resources.cpp',
	'midicube/gui/view/AnalogSynthFMView.cpp',
	'midicube/gui/view/AnalogSynthModulatorView.cpp',
	'midicube/gui/view/AnalogSynthOperatorView.cpp',
	'midicube/gui/view/AnalogSynthOscilatorView.cpp',
	'midicube/gui/view/AnalogSynthView.cpp',
	'midicube/gui/view/ArpeggiatorView.cpp',
	'midicube/gui/view/B3OrganView.cpp',
	'midicube/gui/view/EffectView.cpp',
	'midicube/gui/view/LooperView.cpp',
	'midicube/gui/view/MasterEffectView.cpp',
	'midicube/gui/view/MotionSequencerView.cpp',
	'midicube/gui/view/ProgramRenameView.cpp',
	'midicube/gui/view/ProgramView.cpp',
	'midicube/gui/view/SamplerView.cpp',
	'midicube/gui/view/SceneView.cpp',
	'midicube/gui/view/SoundEngineChannelView.cpp',
	'midicube/gui/view/SoundEngineView.cpp',
	
	'midicube/effect/amplifier_simulation.cpp',
	'midicube/effect/bitcrusher.cpp',
	'midicube/effect/chorus.cpp',
	'midicube/effect/compressor.cpp',
	'midicube/effect/delay.cpp',
	'midicube/effect/effect.cpp',
	'midicube/effect/equalizer.cpp',
	'midicube/effect/flanger.cpp',
	'midicube/effect/phaser.cpp',
	'midicube/effect/reverb.cpp',
	'midicube/effect/rotary_speaker.cpp',
	'midicube/effect/tremolo.cpp',
	'midicube/effect/vocoder.cpp',
	'midicube/effect/wahwah.cpp'
]

executable('midi-cube', src, dependencies: deps, link_args: '-latomic')

sampler_deps = [
	dependency('rtaudio'),
	dependency('rtmidi'),
	dependency('sndfile'),
	cxx.find_library('boost_system'),
	cxx.find_library('boost_filesystem')
]

sampler_src = [
	'autosampler-main.cpp',
	'midicube/autosampler.cpp',
	'midicube/audiofile.cpp',
	'midicube/sfz.cpp',
	'midicube/synthesis.cpp'
]

executable('midi-cube-autosampler', sampler_src, dependencies: sampler_deps, link_args: '-latomic')
