project('midi-cube', 'cpp', default_options: ['cpp_std=c++17'])
cxx = meson.get_compiler('cpp')

deps = [
	dependency('rtaudio'),
	dependency('rtmidi'),
	dependency('sndfile'),
	dependency('sfml-system'),
	dependency('sfml-window'),
	dependency('sfml-graphics'),
	cxx.find_library('pthread'),
	cxx.find_library('boost_system'),
	cxx.find_library('boost_filesystem')
]

src = [
	'midicube-main.cpp',
	'midicube/framework/core/plugins/soundengine.cpp',
	'midicube/framework/core/metronome.cpp',
	'midicube/framework/core/midi.cpp',
	'midicube/framework/core/plugin.cpp',
	'midicube/framework/data/binding.cpp',
	'midicube/framework/data/data.cpp',
	'midicube/framework/dsp/envelope.cpp',
	'midicube/framework/dsp/filter.cpp',
	'midicube/framework/dsp/oscilator.cpp',
	'midicube/framework/dsp/pcm.cpp',
	'midicube/framework/dsp/synthesis.cpp',
	'midicube/framework/dsp/wavetable.cpp',
	'midicube/framework/util/audiofile.cpp',
	'midicube/framework/util/audioloader.cpp',
	'midicube/framework/util/sfz.cpp',
	'midicube/framework/util/util.cpp',
	
	'midicube/audio.cpp',
	'midicube/looper.cpp',
	'midicube/midicube.cpp',
	'midicube/midiio.cpp',
	'midicube/program.cpp',

	'midicube/soundengine/asynth.cpp',
	'midicube/soundengine/drums.cpp',
	'midicube/soundengine/organ.cpp',
	'midicube/soundengine/sampler.cpp',
	'midicube/soundengine/soundengine.cpp',
	
	'midicube/framework/gui/control.cpp',
	'midicube/framework/gui/core.cpp',
	'midicube/framework/gui/util.cpp',
	
	'midicube/view/resources.cpp',
	'midicube/view/AnalogSynthFMView.cpp',
	'midicube/view/AnalogSynthModulatorView.cpp',
	'midicube/view/AnalogSynthOperatorView.cpp',
	'midicube/view/AnalogSynthOscilatorView.cpp',
	'midicube/view/AnalogSynthView.cpp',
	'midicube/view/ArpeggiatorView.cpp',
	'midicube/view/B3OrganView.cpp',
	'midicube/view/BooleanMidiBindingView.cpp',
	'midicube/view/EffectView.cpp',
	'midicube/view/LooperView.cpp',
	'midicube/view/MasterEffectView.cpp',
	'midicube/view/MotionSequencerView.cpp',
	'midicube/view/ProgramRenameView.cpp',
	'midicube/view/ProgramView.cpp',
	'midicube/view/SamplerView.cpp',
	'midicube/view/SceneView.cpp',
	'midicube/view/SoundEngineChannelView.cpp',
	'midicube/view/SoundEngineView.cpp',
	'midicube/view/SourceView.cpp',
	'midicube/view/TemplateMidiBindingView.cpp',
	
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
	'midicube/audio.cpp',
	'midicube/framework/util/audiofile.cpp',
	'midicube/framework/dsp/filter.cpp',
	'midicube/framework/util/sfz.cpp',
	'midicube/framework/dsp/synthesis.cpp'
]

executable('midi-cube-autosampler', sampler_src, dependencies: sampler_deps, link_args: '-latomic')
