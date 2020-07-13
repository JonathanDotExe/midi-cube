/*
 * presetsynth.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "presetsynth.h"
#include "../synthesis.h"


//PresetSynth
PresetSynth::PresetSynth() {
	detune = note_to_freq_transpose(0.1);
	ndetune = note_to_freq_transpose(-0.1);
	vibrato = 0;
	filter = LowPassFilter(20000);
}

void PresetSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note) {
	double sample = 0.0;
	double freq = note.freq;
	double t = info.time + note.phase_shift;
	sample += saw_wave(t, freq);
	sample += saw_wave(t, freq * detune);
	sample += saw_wave(t, freq * ndetune);

	if (vibrato) {
		note.phase_shift += info.time_step * (note_to_freq_transpose(SYNTH_VIBRATO_DETUNE * sine_wave(info.time, SYNTH_VIBRATO_RATE) * vibrato) - 1);
	}

	double amp = env.amplitude(info.time, note);
	sample *= 0.1 * amp;
	sample = filter.apply(sample, info.time_step);

	for (size_t i = 0; i < channels.size() ; ++i) {
		channels[i] += sample;
	}
}

bool PresetSynth::note_finished(SampleInfo& info, TriggeredNote& note) {
	return !note.pressed && note.release_time + env.release < info.time;
}

void PresetSynth::control_change(unsigned int control, unsigned int value) {
	if (control == 1) {
		vibrato = value/127.0;
	}
	if (control == 2) {
		std::cout << filter.get_cutoff() << std::endl;
		filter.set_cutoff(21000/(128.0 - value));
	}
}

std::string PresetSynth::get_name() {
	return "Preset Synth";
}
