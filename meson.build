project('midi-cube', 'cpp')
cxx = meson.get_compiler('cpp')

deps = [
	dependency('rtaudio'),
	dependency('rtmidi'),
	dependency('sndfile'),
	dependency('fluidsynth'),
	cxx.find_library('boost_system'),
	cxx.find_library('boost_filesystem')
]

src = [
	'midicube-main.cpp',
	
	'midicube/audio.cpp',
	'midicube/audiofile.cpp',
	'midicube/envelope.cpp',
	'midicube/filter.cpp',
	'midicube/looper.cpp',
	'midicube/metronome.cpp',
	'midicube/midi.cpp',
	'midicube/midicube.cpp',
	'midicube/oscilator.cpp',
	'midicube/property.cpp',
	'midicube/synthesis.cpp',
	'midicube/util.cpp',
	
	'midicube/soundengine/asynth.cpp',
	'midicube/soundengine/drums.cpp',
	'midicube/soundengine/organ.cpp',
	'midicube/soundengine/sampler.cpp',
	'midicube/soundengine/sfsynth.cpp',
	'midicube/soundengine/soundengine.cpp',
	
	'midicube/gui/engine/control.cpp',
	'midicube/gui/engine/core.cpp',
	'midicube/gui/engine/util.cpp',
	
	'midicube/gui/view/ArpeggiatorView.cpp',
	'midicube/gui/view/B3OrganViewView.cpp',
	'midicube/gui/view/SoundEngineChannelView.cpp',
	'midicube/gui/view/SoundengineView.cpp',
	
	'midicube/effect/bitcrusher.cpp',
	'midicube/effect/vocoder.cpp'
]

executable('midi-cube', src, dependencies: deps)