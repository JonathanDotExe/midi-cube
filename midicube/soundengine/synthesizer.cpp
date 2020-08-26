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
double OscilatorComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Pitch and FM
	phase_shift.at(note_index) += info.time_step * (note_to_freq_transpose(pitch) - 1) + info.time_step * fm;
	//Frequency
	double time = info.time + note.phase_shift + phase_shift.at(note_index);
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

static std::vector<std::string> amplitude_properties = {"Input", "Amplitude"};

#define AMP_ENVELOPE_INPUT_PROPERTY 0
#define AMP_ENVELOPE_AMPLITUDE_PROPERTY 1

//AmplitudeComponent
double AmpEnvelopeComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return input * amplitude_mod * envelope.amplitude(info.time, note, env);
}

void AmpEnvelopeComponent::set_property(size_t index, double value) {
	switch (index) {
	case AMP_ENVELOPE_INPUT_PROPERTY:
		input = value;
		break;
	case AMP_ENVELOPE_AMPLITUDE_PROPERTY:
		amplitude_mod = value;
		break;
	}
}

void AmpEnvelopeComponent::add_property(size_t index, double value) {
	switch (index) {
	case AMP_ENVELOPE_INPUT_PROPERTY:
		input += value;
		break;
	case AMP_ENVELOPE_AMPLITUDE_PROPERTY:
		amplitude_mod += value;
		break;
	}
}

void AmpEnvelopeComponent::mul_property(size_t index, double value) {
	switch (index) {
	case AMP_ENVELOPE_INPUT_PROPERTY:
		input *= value;
		break;
	case AMP_ENVELOPE_AMPLITUDE_PROPERTY:
		amplitude_mod *= value;
		break;
	}
}

double AmpEnvelopeComponent::from() {
	return -1;
}

double AmpEnvelopeComponent::to() {
	return 1;
}
void AmpEnvelopeComponent::reset_properties() {
	amplitude_mod = amplitude;
	input = 0;
}

bool AmpEnvelopeComponent::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	return envelope.is_finished(info.time, note, env);
}

std::vector<std::string> AmpEnvelopeComponent::property_names() {
	return amplitude_properties;
}
size_t AmpEnvelopeComponent::property_count() {
	return amplitude_properties.size();
}

//ModEnvelopeComponent
double ModEnvelopeComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return amplitude * envelope.amplitude(info.time, note, env);
}
void ModEnvelopeComponent::set_property(size_t index, double value) {

}
void ModEnvelopeComponent::add_property(size_t index, double value) {

}
void ModEnvelopeComponent::mul_property(size_t index, double value) {

}

double ModEnvelopeComponent::from() {
	return 0;
}
double ModEnvelopeComponent::to() {
	return 1;
}

void ModEnvelopeComponent::reset_properties() {

}

std::vector<std::string> ModEnvelopeComponent::property_names() {
	return {};
}

size_t ModEnvelopeComponent::property_count() {
	return 0;
}

//ComponentSlot
double ComponentSlot::process(std::array<ComponentSlot, MAX_COMPONENTS>& slots, std::array<double, MAX_COMPONENTS>& values, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	double sample = 0;
	if (comp) {
		comp->reset_properties();
		//Apply bindings
		for (size_t i = 0; i < bindings.size(); ++i) {
			double value = values.at(bindings[i].component);
			ComponentSlot& slot = slots.at(bindings[i].component);

			double from = slot.from();
			double to = slot.to();
			double prog = (value - from)/(to - from);
			value = prog * comp->to() + (1 - prog) * comp->from();

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
		sample = comp->process(info, note, env, note_index);
	}
	return sample;
}

double ComponentSlot::from() {
	return comp ? comp->from() : 0;;
}

double ComponentSlot::to() {
	return comp ? comp->to() : 0;
}

void ComponentSlot::set_component(SynthComponent* comp) {
	delete this->comp;
	this->comp = comp;
}

bool ComponentSlot::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	return comp ? comp->note_finished(info, note, env) : true;
}

ComponentSlot::~ComponentSlot() {
	set_component(nullptr);
}

//SynthesizerData
SynthesizerData::SynthesizerData() {
	//Patch 1 -- Unison Saw Lead/Brass
	OscilatorComponent* comp = new OscilatorComponent();
	comp->osc.set_waveform(AnalogWaveForm::SAW);
	comp->unison_amount = 2;
	comp->unison_detune = 0.1;
	preset.components[10].set_component(comp);

	AmpEnvelopeComponent* amp = new AmpEnvelopeComponent();
	amp->envelope = {0.1, 0, 1, 0.3};

	preset.components[11].set_component(amp);
	preset.components[11].bindings.push_back({BindingType::ADD, AMP_ENVELOPE_INPUT_PROPERTY, 10, -1, 1});
	preset.components[11].audible = true;

	//Patch 2 -- Simple FM Keys
	/*OscilatorComponent* comp1 = new OscilatorComponent();
	comp1->osc.set_waveform(AnalogWaveForm::SINE);
	comp1->volume = 2;
	preset.components[0].set_component(comp1);

	OscilatorComponent* comp2 = new OscilatorComponent();
	comp2->osc.set_waveform(AnalogWaveForm::SINE);

	preset.components[1].set_component(comp2);
	preset.components[1].audible = true;
	preset.components[1].bindings.push_back({BindingType::ADD, OSCILATOR_FM_PROPERTY, 0, -1, 1});*/
}

//Synthesizer
Synthesizer::Synthesizer() {
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
	/*OscilatorSlot* osc = new OscilatorSlot(new AnalogOscilator(AnalogWaveForm::SAW));
	osc->set_unison(3);
	std::vector<FilterData> filters;
	filters.push_back({FilterType::LOW_PASS, 1200});
	preset->oscilators.push_back({osc, {0.1, 0, 1, 0.3}, filters});*/
}

/*static double apply_filter (FilterData& data, FilterInstance& inst, double sample, double time_step) {
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
}*/

void Synthesizer::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& d, size_t note_index) {
	SynthesizerData& data = dynamic_cast<SynthesizerData&>(d);

	double sample = 0;
	//Process components
	std::array<double, MAX_COMPONENTS> values = {0};
	for (size_t i = 0; i < data.preset.components.size(); ++i) {
		double value = data.preset.components[i].process(data.preset.components, values, info, note, env, note_index);
		values[i] = value;

		if (data.preset.components[i].audible) {
			sample += value;
		}
	}
	//Play samples
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void Synthesizer::note_not_pressed(SampleInfo& info, TriggeredNote& note, SoundEngineData& data, size_t note_index) {

}

void Synthesizer::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& data) {

}

void Synthesizer::control_change(unsigned int control, unsigned int value, SoundEngineData& data) {

}

bool Synthesizer::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& d) {
	SynthesizerData& data = dynamic_cast<SynthesizerData&>(d);

	for (size_t i = 0; i < data.preset.components.size(); ++i) {
		if (!data.preset.components[i].note_finished(info, note, env)) {
			return false;
		}
	}
	return true;
}

std::string Synthesizer::get_name() {
	return "Synthesizer";
}


Synthesizer::~Synthesizer() {

}

