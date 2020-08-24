/*
 * synthesizer.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "synthesizer.h"
#include <cmath>

static std::vector<std::string> oscilator_properties = {"Amplitude", "Sync", "FM", "Pitch", "Unison-Detune"};

#define OSCILATOR_VOLUME_PROPERTY 0
#define OSCILATOR_SYNC_PROPERTY 1
#define OSCILATOR_FM_PROPERTY 2
#define OSCILATOR_PITCH_PROPERTY 3
#define OSCILATOR_UNISON_DETUNE_PROPERTY 4

//OscilatorComponent
double OscilatorComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	//Pitch and FM
	phase_shift += info.time_step * note_to_freq_transpose(pitch) + info.time_step * fm;
	//Frequency
	double time = info.time + note.phase_shift + phase_shift;
	double freq = note.freq;
	if (semi) {
		freq *= note_to_freq_transpose(semi);
	}
	freq *= transpose;

	//Sync
	if (sync_mod != 1) {
		time = fmod(time, freq * sync_mod);
	}
	//Signal
	double signal = osc.signal(time, freq);
	//Unison
	if (unison_amount) {
		double udetune = note_to_freq_transpose(unison_detune_mod);
		double nudetune = note_to_freq_transpose(unison_detune_mod);
		double det = udetune;
		double ndet = nudetune;
		for (unsigned int i = 1; i <= unison_amount; ++i) {
			if (i % 2 == 0) {
				signal += osc.signal(time, freq * ndet);
				ndet *= nudetune;
			}
			else {
				signal += osc.signal(time, freq * det);
				det *= udetune;
			}
		}
	}
	return signal / (1 + unison_amount) * amplitude * volume;
}

void OscilatorComponent::set_property(size_t index, double value) {
	switch (index) {
	case OSCILATOR_VOLUME_PROPERTY:
		amplitude = value;
		break;
	case OSCILATOR_SYNC_PROPERTY:
		sync_mod = value;
		break;
	case OSCILATOR_FM_PROPERTY:
		fm = value;
		break;
	case OSCILATOR_PITCH_PROPERTY:
		pitch = value;
		break;
	case OSCILATOR_UNISON_DETUNE_PROPERTY:
		unison_detune_mod = value;
		break;
	}
}

void OscilatorComponent::add_property(size_t index, double value) {
	switch (index) {
		case OSCILATOR_VOLUME_PROPERTY:
			amplitude += value;
			break;
		case OSCILATOR_SYNC_PROPERTY:
			sync_mod += value;
			break;
		case OSCILATOR_FM_PROPERTY:
			fm += value;
			break;
		case OSCILATOR_PITCH_PROPERTY:
			pitch += value;
			break;
		case OSCILATOR_UNISON_DETUNE_PROPERTY:
			unison_detune_mod += value;
			break;
	}
}

void OscilatorComponent::mul_property(size_t index, double value) {
	switch (index) {
		case OSCILATOR_VOLUME_PROPERTY:
			amplitude *= value;
			break;
		case OSCILATOR_SYNC_PROPERTY:
			sync_mod *= value;
			break;
		case OSCILATOR_FM_PROPERTY:
			fm *= value;
			break;
		case OSCILATOR_PITCH_PROPERTY:
			pitch *= value;
			break;
		case OSCILATOR_UNISON_DETUNE_PROPERTY:
			unison_detune_mod *= value;
			break;
	}
}

void OscilatorComponent::reset_properties() {
	amplitude = 1;
	sync_mod = sync;
	fm = 0;
	pitch = 0;
	unison_detune_mod = unison_detune;
}

std::vector<std::string> OscilatorComponent::property_names() {
	return oscilator_properties;
}

size_t OscilatorComponent::property_count() {
	return oscilator_properties.size();
}

double OscilatorComponent::from() {
	return -1;
}

double OscilatorComponent::to() {
	return 1;
}

double ComponentSlot::process(std::array<ComponentSlot, MAX_COMPONENTS>& slots, std::array<double, MAX_COMPONENTS>& values, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	if (comp) {
		//Apply bindings
		for (size_t i = 0; i < bindings.size(); ++i) {
			double value = values.at(bindings[i].component);
			ComponentSlot& slot = slots.at(bindings[i].component);

			double prog = value/(slot.value_range());
			value = prog * comp->to() + (1 - comp) * comp->from();

			//Update property
			switch (bindings[i].type) {
			case BindingType::SET:
				comp->set_property(bindings[i].property, value);
				break;
			case BindingType::ADD:
				comp->add_property(bindings[i].property, value);
				break;
			case BindingType::MUL:
				comp->mul_property(bindings[i].property, value);
				break;
			}
		}

		//Process
		comp->process(info, note, env);
	}
	return 0;
}

double ComponentSlot::value_range() {
	return comp ? (comp->to() - comp->from()) : 0;
}

void ComponentSlot::set_component(SynthComponent* comp) {
	delete comp;
	this->comp = comp;
}

SynthComponent* ComponentSlot::get_component() {
	return comp;
}

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
	filters.push_back({FilterType::LOW_PASS, 1200});
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

void Synthesizer::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index) {
	//Envelopes
	std::vector<double> env_val;
	for (size_t i = 0; i < preset->envelopes.size(); ++i) {
		env_val.push_back(preset->envelopes[i].amplitude(info.time, note, env));
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
		double env_val = osc.env.amplitude(info.time, note, env);
		s *= env_val;

		sample += s;
	}
	//Play samples
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void Synthesizer::note_not_pressed(SampleInfo& info, TriggeredNote& note, SoundEngineData& data, size_t note_index) {
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

void Synthesizer::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& data) {
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

void Synthesizer::control_change(unsigned int control, unsigned int value, SoundEngineData& data) {
	//Control bindings
	for (size_t i = 0; i < preset->control_bindings.size() ; ++i) {
		OscilatorSlot* slot = preset->oscilators.at(preset->control_bindings[i].oscilator).osc;
		double val = preset->control_bindings[i].from + (preset->control_bindings[i].to - preset->control_bindings[i].from) * value/127.0;
		slot->set_property(preset->control_bindings[i].property, val);
	}
}

bool Synthesizer::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data) {
	return !note.pressed && (!env.sustain || env.sustain_time > note.release_time) && fmax(note.release_time, env.sustain_release_time) + release_time < info.time;
}

std::string Synthesizer::get_name() {
	return "Synthesizer";
}


Synthesizer::~Synthesizer() {
	delete preset;
	preset = nullptr;
}

