project('midi-cube', 'cpp', default_options: ['cpp_std=gnu++17'])
cxx = meson.get_compiler('cpp')

deps = [
	dependency('rtaudio'),
	dependency('rtmidi'),
	dependency('sndfile'),
	dependency('sfml-system'),
	dependency('sfml-window'),
	dependency('sfml-graphics'),
	dependency('threads'),
	dependency('boost'),
	cxx.find_library('boost_system', required: false),
	cxx.find_library('boost_filesystem', required: false)
]

src = [
	'midicube-main.cpp',
	'midicube/framework/core/plugins/effect.cpp',
	'midicube/framework/core/plugins/soundengine.cpp',
	'midicube/framework/core/control.cpp',
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
	'midicube/framework/util/clipboard.cpp',
	'midicube/framework/util/sfz.cpp',
	'midicube/framework/util/util.cpp',
	'midicube/framework/gui/control.cpp',
	'midicube/framework/gui/core.cpp',
	'midicube/framework/gui/frame.cpp',
	'midicube/framework/gui/util.cpp',
	'midicube/framework/gui/binding/BooleanMidiBindingView.cpp',
	'midicube/framework/gui/binding/TemplateMidiBindingView.cpp',
	
	'midicube/workstation/audio.cpp',
	'midicube/workstation/midicube.cpp',
	'midicube/workstation/midiio.cpp',
	'midicube/workstation/program.cpp',
	'midicube/workstation/soundengine.cpp',

	'midicube/plugins/soundengine/asynth.cpp',
	'midicube/plugins/soundengine/drums.cpp',
	'midicube/plugins/soundengine/organ.cpp',
	'midicube/plugins/soundengine/sampler.cpp',
	'midicube/plugins/sequencer/arpeggiator.cpp',
	
	'midicube/plugins/resources.cpp',
	'midicube/workstation/view/MasterEffectView.cpp',
	'midicube/workstation/view/PluginSelectView.cpp',
	'midicube/workstation/view/PluginView.cpp',
	'midicube/workstation/view/ProgramRenameView.cpp',
	'midicube/workstation/view/ProgramView.cpp',
	'midicube/workstation/view/SceneView.cpp',
	'midicube/workstation/view/SoundEngineChannelView.cpp',
	'midicube/workstation/view/SoundEngineView.cpp',
	'midicube/workstation/view/SourceView.cpp',
	
	'midicube/plugins/effect/amplifier_simulation.cpp',
	'midicube/plugins/effect/bitcrusher.cpp',
	'midicube/plugins/effect/chorus.cpp',
	'midicube/plugins/effect/compressor.cpp',
	'midicube/plugins/effect/delay.cpp',
	'midicube/plugins/effect/equalizer.cpp',
	'midicube/plugins/effect/flanger.cpp',
	'midicube/plugins/effect/looper.cpp',
	'midicube/plugins/effect/phaser.cpp',
	'midicube/plugins/effect/reverb.cpp',
	'midicube/plugins/effect/rotary_speaker.cpp',
	'midicube/plugins/effect/tremolo.cpp',
	'midicube/plugins/effect/vocoder.cpp',
	'midicube/plugins/effect/wahwah.cpp',
	
	'midicube/plugins/view/AnalogSynthFMView.cpp',
	'midicube/plugins/view/AnalogSynthModulatorView.cpp',
	'midicube/plugins/view/AnalogSynthOperatorView.cpp',
	'midicube/plugins/view/AnalogSynthOscilatorView.cpp',
	'midicube/plugins/view/AnalogSynthView.cpp',
	'midicube/plugins/view/ArpeggiatorView.cpp',
	'midicube/plugins/view/B3OrganView.cpp',
	'midicube/plugins/view/EffectView.cpp',
	'midicube/plugins/view/LooperView.cpp',
	'midicube/plugins/view/SamplerView.cpp'
]

executable('midi-cube', src, dependencies: deps, link_args: '-latomic')

sampler_deps = [
	dependency('rtaudio'),
	dependency('rtmidi'),
	dependency('sndfile'),
	dependency('boost'),
	cxx.find_library('boost_system', required: false),
	cxx.find_library('boost_filesystem', required: false)
]

sampler_src = [
	'autosampler-main.cpp',
	'midicube/workstation/autosampler.cpp',
	'midicube/workstation/audio.cpp',
	'midicube/framework/util/audiofile.cpp',
	'midicube/framework/dsp/filter.cpp',
	'midicube/framework/util/sfz.cpp',
	'midicube/framework/dsp/synthesis.cpp'
]

executable('midi-cube-autosampler', sampler_src, dependencies: sampler_deps, link_args: '-latomic')
