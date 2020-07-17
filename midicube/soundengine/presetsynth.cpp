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
	filter.set_cutoff(21000);

	AdditiveOscilator* aosc = new AdditiveOscilator();
	std::vector<double> harmonics = { 0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8 };
	for (size_t i = 0; i < harmonics.size(); ++i) {
		aosc->add_sine({harmonics[i], 1});
	}

	osc = new OscilatorSlot(aosc);
	osc->set_volume(0.3);
	osc->set_unison(2);
	osc->set_unison_detune(0.05);

	osc2 = new OscilatorSlot(new AnalogOscilator(AnalogWaveForm::SAW));
	osc2->set_volume(0.05);
	osc2->set_unison(2);
}

void PresetSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note) {
	double sample = 0.0;
	double freq = note.freq;
	double t = info.time + note.phase_shift;
	sample = osc->signal(t, freq) + osc2->signal(t, freq);

	if (vibrato) {
		note.phase_shift += info.time_step * (note_to_freq_transpose(SYNTH_VIBRATO_DETUNE * sine_wave(info.time, SYNTH_VIBRATO_RATE) * vibrato) - 1);
	}

	double amp = env.amplitude(info.time, note);
	sample *= amp;

	for (size_t i = 0; i < channels.size() ; ++i) {
		channels[i] += sample;
	}
}

void PresetSynth::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	filter.apply(channels, info.time_step);
};

bool PresetSynth::note_finished(SampleInfo& info, TriggeredNote& note) {
	return !note.pressed && note.release_time + env.release < info.time;
}

void PresetSynth::control_change(unsigned int control, unsigned int value) {
	if (control == 1) {
		vibrato = value/127.0;
	}
	if (control == 2) {
		filter.set_cutoff(21000/(128.0 - value));
	}
}

std::string PresetSynth::get_name() {
	return "Preset Synth";
}

PresetSynth::~PresetSynth() {
	delete osc;
	osc = nullptr;
	delete osc2;
	osc2 = nullptr;
}

