/*
 * presetsynth.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "presetsynth.h"
#include "../synthesis.h"
#include <iostream>
#include <cmath>

/*
//PresetSynth
FMSynth::FMSynth() {
	/*detune = note_to_freq_transpose(0.1);
	ndetune = note_to_freq_transpose(-0.1);
	vibrato = 0;
	filter.set_cutoff(6300);
)
	AdditiveOscilator* aosc = new AdditiveOscilator();
	std::vector<double> harmonics = { 0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8 };
	for (size_t i = 0; i < harmonics.size(); ++i) {
		//aosc->add_sine({harmonics[i], 1});
	}

	osc = new OscilatorSlot(aosc);
	osc->set_volume(0.0);
	osc->set_unison(3);
	osc->set_unison_detune(0.05);

	osc2 = new OscilatorSlot(new AnalogOscilator(AnalogWaveForm::SQUARE));
	osc2->set_volume(1);
	osc2->set_unison(0);

	detune = note_to_freq_transpose(0.1);
	ndetune = note_to_freq_transpose(-0.1);
	vibrato = 0;
	//filter.set_cutoff(1200);

	/*osc.data.analog = true;
	osc.data.waveform = AnalogWaveForm::SAW_DOWN;
	osc.unison_amount = 3;

	phase = 0;
	fm_phase = 0;
}

void FMSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	double sample = 0.0;
	double freq = note.freq * env.pitch_bend;
	double fm_freq = note.freq * env.pitch_bend;
	/*sample = osc.signal(freq, info.time_step, note_index);

	if (vibrato) {
		note.phase_shift += info.time_step * (note_to_freq_transpose(SYNTH_VIBRATO_DETUNE * sine_wave(info.time, SYNTH_VIBRATO_RATE) * vibrato) - 1);
	}

	double amp = this->env.amplitude(info.time, note, env);
	sample *= amp;*/

	/*fm_phase += info.time_step * fm_freq;
	phase += freq * info.time_step;
	double mod = sine_wave(fm_phase, 1) * vibrato;
	double mul = (freq + mod)/freq;

	sample = sine_wave(phase * mul, 1);

	sample = sine_wave(info.time, freq + sine_wave(info.time, fm_freq) * vibrato);

	for (size_t i = 0; i < channels.size() ; ++i) {
		channels[i] += sample;
	}
}

void FMSynth::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {
	//filter.apply(channels, info.time_step);
};

bool FMSynth::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return this->env.is_finished(info.time, note, env);
}

void FMSynth::control_change(unsigned int control, unsigned int value) {
	if (control == 1) {
		vibrato = value/127.0;
	}
}


template<>
std::string get_engine_name<FMSynth>() {
	return "Preset Synth";
}

void __fix_link_preset_synth_name__ () {
	get_engine_name<FMSynth>();
}

FMSynth::~FMSynth() {

}*/

