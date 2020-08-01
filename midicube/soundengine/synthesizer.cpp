/*
 * synthesizer.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "synthesizer.h"

Synthesizer::Synthesizer() {
	preset = new SynthesizerPreset();

	//Patch 1 -- Unison Saw Lead
	/*OscilatorSlot* osc = new OscilatorSlot(new AnalogOscilator(AnalogWaveForm::SAW));
	osc->set_unison(2);
	preset->oscilators.push_back({osc, {0.0005, 0, 1, 0.0005}});

	Filter* filter = new LowPassFilter(21000);
	preset->filters.push_back({filter, 0});*/

	//Patch 2 -- Saw Bass
	/*OscilatorSlot* osc = new OscilatorSlot(new AnalogOscilator(AnalogWaveForm::SAW));
	std::vector<FilterData> filters;
	filters.push_back({FilterType::LOW_PASS, 6300});
	preset->oscilators.push_back({osc, {0.0005, 0, 1, 0.0005}, filters});*/


	//Patch 3 -- Simple Saw Pad
	OscilatorSlot* osc = new OscilatorSlot(new AnalogOscilator(AnalogWaveForm::SAW));
	osc->set_unison(3);
	std::vector<FilterData> filters;
	filters.push_back({FilterType::LOW_PASS, 200});
	preset->oscilators.push_back({osc, {0.1, 0, 1, 0.3}, filters});

	//Calc release time
	release_time = 0;
	for (size_t i = 0; i < preset->oscilators.size(); ++i) {
		if (preset->oscilators[i].env.release > release_time) {
			release_time = preset->oscilators[i].env.release;
		}
	}
}

static double apply_filter (FilterData& data, FilterInstance& inst, double sample, double time_step) {
	double last = sample;
	switch (data.type) {
	case FilterType::LOW_PASS:
		sample = apply_low_pass(sample, time_step, cutoff_to_rc(data.cutoff), inst.last_filtered);
		break;
	case FilterType::HIGH_PASS:
		sample = apply_high_pass(sample, time_step, cutoff_to_rc(data.cutoff), inst.last_filtered, inst.last, inst.started);
		break;
	}
	inst.last = last;
	inst.last_filtered = sample;
	inst.started = true;
	return sample;
}

void Synthesizer::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, size_t note_index) {
	//Envelopes
	std::vector<double> env_val;
	for (size_t i = 0; i < preset->envelopes.size(); ++i) {
		env_val.push_back(preset->envelopes[i].amplitude(info.time, note));
	}
	//Envelope Bindings
	for (size_t i = 0; i < preset->envelope_bindings.size(); ++i) {
		OscilatorSlot* osc = preset->oscilators.at(preset->envelope_bindings[i].oscilator).osc;
		double value = env_val.at(preset->envelope_bindings[i].envelope);
		double val = preset->envelope_bindings[i].from + (preset->envelope_bindings[i].to - preset->envelope_bindings[i].from) * value;
		osc->set_property(preset->envelope_bindings[i].property, val);
	}
	double sample = 0;
	//Oscilator samples
	for (size_t i = 0; i < preset->oscilators.size() && i < filters.size(); ++i) {
		OscilatorEnvelope& osc = preset->oscilators[i];
		//Signal
		double s = osc.osc->signal(info.time + note.phase_shift, note.freq);
		//Filter
		for (size_t j = 0; j < osc.filters.size(); ++j) {
			s = apply_filter(osc.filters.at(j), filters[i].at(note_index)[0], s, info.time_step);
			s = apply_filter(osc.filters.at(j), filters[i].at(note_index)[1], s, info.time_step);
		}
		//Envelope
		double env = osc.env.amplitude(info.time, note);
		s *= env;

		sample += s * 0.3;
	}
	//Play samples
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void Synthesizer::note_not_pressed(SampleInfo& info, TriggeredNote& note, size_t note_index) {
	for (size_t i = 0; i < preset->oscilators.size() && i < filters.size(); ++i) {
		OscilatorEnvelope& osc = preset->oscilators[i];
		//Signal
		double s = 0;
		//Filter
		for (size_t j = 0; j < osc.filters.size(); ++j) {
			s = apply_filter(osc.filters.at(j), filters[i].at(note_index)[0], s, info.time_step);
			s = apply_filter(osc.filters.at(j), filters[i].at(note_index)[1], s, info.time_step);
		}
	}
}

void Synthesizer::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	//Filters
	/*for (size_t i = 0; i < preset->filters.size(); ++i) {
		double filtered  = preset->filters[i].filter->apply(samples.at(preset->filters[i].osc), info.time_step);
		samples[preset->filters[i].osc] = filtered;
	}
	//Play samples
	for (size_t i = 0; i < channels.size(); ++i) {
		for (size_t j = 0; j < samples.size(); ++j) {
			channels[i] += samples[j] * 0.3;
		}
	}
	//Reset samples
	for (size_t i = 0; i < samples.size(); ++i) {
		samples[i] = 0;
	}*/
}

void Synthesizer::control_change(unsigned int control, unsigned int value) {
	//Control bindings
	for (size_t i = 0; i < preset->control_bindings.size() ; ++i) {
		OscilatorSlot* slot = preset->oscilators.at(preset->control_bindings[i].oscilator).osc;
		double val = preset->control_bindings[i].from + (preset->control_bindings[i].to - preset->control_bindings[i].from) * value/127.0;
		slot->set_property(preset->control_bindings[i].property, val);
	}
}

bool Synthesizer::note_finished(SampleInfo& info, TriggeredNote& note) {
	return !note.pressed && note.release_time + release_time < info.time;
}

std::string Synthesizer::get_name() {
	return "Synthesizer";
}


Synthesizer::~Synthesizer() {
	delete preset;
	preset = nullptr;
}

