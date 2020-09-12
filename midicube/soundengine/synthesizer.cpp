/*
 * synthesizer.cpp
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#include "synthesizer.h"
#include <cmath>
#include <algorithm>
#include <iostream>

static std::vector<std::string> oscilator_properties = {"Amplitude", "Sync", "FM", "Pitch", "Unison-Detune", "Pulse Width"};

#define OSCILATOR_VOLUME_PROPERTY 0
#define OSCILATOR_SYNC_PROPERTY 1
#define OSCILATOR_FM_PROPERTY 2
#define OSCILATOR_PITCH_PROPERTY 3
#define OSCILATOR_UNISON_DETUNE_PROPERTY 4
#define OSCILATOR_PULSE_WIDTH_PROPERTY 5


//OscilatorComponent
OscilatorComponent::OscilatorComponent() {
	osc.data.analog = true;
}

double OscilatorComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Data
	osc.data.pulse_width = pulse_width_mod;
	osc.data.sync_mul = sync_mod;
	osc.unison_detune = unison_detune_mod;
	//Pitch and FM
	//Frequency
	double freq = note.freq * env.pitch_bend * transpose * fm;
	if (semi) {
		freq *= note_to_freq_transpose(semi);
	}
	if (pitch) {
		freq *= note_to_freq_transpose(pitch);
	}

	//Signal
	double signal = osc.signal(freq, info.time_step, note_index);
	return amplitude * volume * signal;
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
	case OSCILATOR_PULSE_WIDTH_PROPERTY:
		pulse_width_mod = value;
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
		case OSCILATOR_PULSE_WIDTH_PROPERTY:
			pulse_width_mod += value;
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
		case OSCILATOR_PULSE_WIDTH_PROPERTY:
			pulse_width_mod *= value;
			break;
	}
}

void OscilatorComponent::reset_properties() {
	amplitude = 1;
	sync_mod = sync;
	fm = 1;
	pitch = 0;
	unison_detune_mod = unison_detune;
	pulse_width_mod = pulse_width;
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

std::vector<std::string> mod_envelope_properties = {"Amplitude"};
#define MOD_ENVELOPE_AMPLITUDE_PROPERTY 0

//ModEnvelopeComponent
double ModEnvelopeComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return amplitude_mod * envelope.amplitude(info.time, note, env);
}
void ModEnvelopeComponent::set_property(size_t index, double value) {
	switch (index) {
	case MOD_ENVELOPE_AMPLITUDE_PROPERTY:
		amplitude_mod = value;
		break;
	}
}
void ModEnvelopeComponent::add_property(size_t index, double value) {
	switch (index) {
	case MOD_ENVELOPE_AMPLITUDE_PROPERTY:
		amplitude_mod += value;
		break;
	}
}
void ModEnvelopeComponent::mul_property(size_t index, double value) {
	switch (index) {
	case MOD_ENVELOPE_AMPLITUDE_PROPERTY:
		amplitude_mod *= value;
		break;
	}
}

double ModEnvelopeComponent::from() {
	return 0;
}
double ModEnvelopeComponent::to() {
	return 1;
}

void ModEnvelopeComponent::reset_properties() {
	amplitude_mod = amplitude;
}

std::vector<std::string> ModEnvelopeComponent::property_names() {
	return mod_envelope_properties;
}

size_t ModEnvelopeComponent::property_count() {
	return mod_envelope_properties.size();
}

static std::vector<std::string> lfo_properties = {"Amplitude"};

#define LFO_AMPLITUDE_PROPERTY 0

//LFOComponent
LFOComponent::LFOComponent() {
	osc.data.waveform = AnalogWaveForm::SINE;
}

double LFOComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return osc.signal(freq, info.time_step, note_index) * amplitude_mod;
}

void LFOComponent::set_property(size_t index, double value){
	switch (index) {
	case LFO_AMPLITUDE_PROPERTY:
		amplitude_mod = value;
		break;
	}
}

void LFOComponent::add_property(size_t index, double value){
	switch (index) {
	case LFO_AMPLITUDE_PROPERTY:
		amplitude_mod += value;
		break;
	}
}

void LFOComponent::mul_property(size_t index, double value){
	switch (index) {
	case LFO_AMPLITUDE_PROPERTY:
		amplitude_mod *= value;
		break;
	}
}

double LFOComponent::from(){
	return -1;
}

double LFOComponent::to(){
 return 1;
}

void LFOComponent::reset_properties(){
	amplitude_mod = amplitude;
}

std::vector<std::string> LFOComponent::property_names(){
	return lfo_properties;
}

size_t LFOComponent::property_count(){
	return lfo_properties.size();
}

//ControlChangeComponent
double ControlChangeComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return value;
}

void ControlChangeComponent::set_property(size_t index, double value) {

}

void ControlChangeComponent::add_property(size_t index, double value) {

}

void ControlChangeComponent::mul_property(size_t index, double value) {

}

double ControlChangeComponent::from() {
	return 0;
}

double ControlChangeComponent::to() {
	return 1;
}

void ControlChangeComponent::control_change(unsigned int control, unsigned int value) {
	if (control == this->control) {
		value = std::min(end, std::max(start, value));
		this->value = (double) value/(end - start);
	}
}

void ControlChangeComponent::reset_properties() {

}

std::vector<std::string> ControlChangeComponent::property_names() {
	return {};
}

size_t ControlChangeComponent::property_count() {
	return 0;
}

//VelocityComponent
double VelocityComponent::process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return (double) std::min(end, std::max(start, note.velocity));
}

void VelocityComponent::set_property(size_t index, double value) {

}

void VelocityComponent::add_property(size_t index, double value) {

}

void VelocityComponent::mul_property(size_t index, double value) {

}

double VelocityComponent::from() {
	return 0;
}

double VelocityComponent::to() {
	return 1;
}

void VelocityComponent::reset_properties() {

}

std::vector<std::string> VelocityComponent::property_names() {
	return {};
}

size_t VelocityComponent::property_count() {
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
			value = prog * bindings[i].to + (1 - prog) * bindings[i].from;

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

void ComponentSlot::note_not_pressed(SampleInfo& info, TriggeredNote& note, size_t note_index) {
	if (comp) {
		comp->note_not_pressed(info, note, note_index);
	}
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

SynthComponent* ComponentSlot::get_component() {
	return comp;
}

void ComponentSlot::control_change(unsigned int control, unsigned int value) {
	if (comp) {
		comp->control_change(control, value);
	}
}

bool ComponentSlot::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	return comp ? comp->note_finished(info, note, env) : true;
}

ComponentSlot::~ComponentSlot() {
	set_component(nullptr);
}

//SynthesizerData
static void apply_preset(SynthesizerPreset& preset, unsigned int preset_no) {
	//Clear patch
	for (size_t i = 0; i < preset.components.size(); ++i) {
		preset.components[i].set_component(nullptr);
		preset.components[i].audible = false;
		preset.components[i].bindings.clear();
	}
	//Apply preset
	switch (preset_no) {
	case 1:
		//Patch 1 -- Unison Saw Lead/Brass
	{
		ModEnvelopeComponent *env = new ModEnvelopeComponent();
		env->envelope = { 0.05, 0, 1, 10 };
		preset.components[0].set_component(env);

		ControlChangeComponent *cc = new ControlChangeComponent();
		cc->control = 1;
		preset.components[1].set_component(cc);

		LFOComponent *lfo = new LFOComponent();
		lfo->freq = 6;
		preset.components[2].set_component(lfo);
		preset.components[2].bindings.push_back( { BindingType::MUL,
				LFO_AMPLITUDE_PROPERTY, 1, 0, 1 });

		OscilatorComponent *comp = new OscilatorComponent();
		comp->osc.data.waveform = AnalogWaveForm::SAW_DOWN;
		comp->osc.unison_amount = 2;
		comp->unison_detune = 0.1;
		preset.components[9].set_component(comp);
		preset.components[9].bindings.push_back( { BindingType::ADD,
				OSCILATOR_PITCH_PROPERTY, 2, -1, 1 });

		LowPassFilter24Component *filter = new LowPassFilter24Component();
		filter->cutoff = 7000;
		preset.components[10].set_component(filter);
		preset.components[10].bindings.push_back( { BindingType::ADD,
				FILTER_INPUT_PROPERTY, 9, -1, 1 });
		preset.components[10].bindings.push_back( { BindingType::ADD,
				FILTER_CUTOFF_PROPERTY, 0, 0, 21000 - 7000 });

		AmpEnvelopeComponent *amp = new AmpEnvelopeComponent();
		amp->envelope = { 0.0005, 0, 1, 0.03 };

		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back( { BindingType::ADD,
				AMP_ENVELOPE_INPUT_PROPERTY, 10, -1, 1 });
		preset.components[11].audible = true;
	}
		break;
	case 2:
		//Patch 2 -- FM Bass
	{
		OscilatorComponent* comp1 = new OscilatorComponent();
		comp1->osc.data.waveform = AnalogWaveForm::SINE;
		comp1->volume = 1;
		comp1->transpose = 2;
		preset.components[0].set_component(comp1);

		OscilatorComponent* comp2 = new OscilatorComponent();
		comp2->osc.data.waveform = AnalogWaveForm::SINE;

		preset.components[1].set_component(comp2);
		preset.components[1].bindings.push_back({BindingType::ADD, OSCILATOR_FM_PROPERTY, 0, -1, 1});

		AmpEnvelopeComponent *amp = new AmpEnvelopeComponent();
		amp->envelope = { 0.0005, 0, 1, 0.03 };

		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back( { BindingType::ADD,
				AMP_ENVELOPE_INPUT_PROPERTY, 1, -1, 1 });
		preset.components[11].audible = true;
	}
		break;
	case 3:
		//Patch 3 -- Simple Saw Pad
	{
		LFOComponent* lfo = new LFOComponent();
		lfo->freq = 0.8;
		preset.components[0].set_component(lfo);

		VelocityComponent* velocity = new VelocityComponent();
		preset.components[1].set_component(velocity);

		OscilatorComponent* comp = new OscilatorComponent();
		comp->osc.data.waveform = AnalogWaveForm::SAW_DOWN;
		comp->osc.unison_amount = 3;
		comp->unison_detune = 0.1;
		preset.components[9].set_component(comp);

		LowPassFilter12Component* filter = new LowPassFilter12Component();
		filter->cutoff = 1200;
		preset.components[10].set_component(filter);
		preset.components[10].bindings.push_back({BindingType::ADD, FILTER_INPUT_PROPERTY, 9, -1, 1});
		preset.components[10].bindings.push_back({BindingType::MUL, FILTER_CUTOFF_PROPERTY, 0, 0.9, 1.1});

		AmpEnvelopeComponent* amp = new AmpEnvelopeComponent();
		amp->envelope = {0.1, 0, 1, 0.3};

		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back({BindingType::ADD, AMP_ENVELOPE_INPUT_PROPERTY, 10, -1, 1});
		preset.components[11].bindings.push_back({BindingType::MUL, AMP_ENVELOPE_AMPLITUDE_PROPERTY, 1, 0.7, 1});
		preset.components[11].audible = true;
	}
		break;
	case 4:
		//Patch 4 -- Sweeping Square Bass
	{
		ControlChangeComponent* cc = new ControlChangeComponent();
		cc->control = 1;
		preset.components[0].set_component(cc);

		ModEnvelopeComponent* env = new ModEnvelopeComponent();
		env->envelope = {0.0005, 0.3, 0, 0.3};
		preset.components[1].set_component(env);
		preset.components[1].bindings.push_back({BindingType::MUL, MOD_ENVELOPE_AMPLITUDE_PROPERTY, 0, 1, 0});

		OscilatorComponent* comp = new OscilatorComponent();
		comp->osc.data.waveform = AnalogWaveForm::SQUARE;
		comp->pulse_width = 1.0/3;
		preset.components[9].set_component(comp);

		LowPassFilter24Component* filter = new LowPassFilter24Component();
		filter->cutoff = 24;
		preset.components[10].set_component(filter);
		preset.components[10].bindings.push_back({BindingType::ADD, FILTER_INPUT_PROPERTY, 9, -1, 1});
		preset.components[10].bindings.push_back({BindingType::ADD, FILTER_CUTOFF_PROPERTY, 1, 0, 6300 - 24});

		AmpEnvelopeComponent* amp = new AmpEnvelopeComponent();
		amp->envelope = {0.0005, 0.6, 0, 0.1};
		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back({BindingType::ADD, AMP_ENVELOPE_INPUT_PROPERTY, 10, -1, 1});
		preset.components[11].audible = true;
	}
		break;
	case 5:
		//Patch 5 -- Soft Pad
	{
		ModEnvelopeComponent* mod = new ModEnvelopeComponent();
		mod->envelope = {1.0, 1.0, 0.2, 0.3};
		mod->amplitude = 0.7;
		preset.components[0].set_component(mod);

		VelocityComponent* velocity = new VelocityComponent();
		preset.components[1].set_component(velocity);

		OscilatorComponent* comp1 = new OscilatorComponent();
		comp1->osc.data.waveform = AnalogWaveForm::SINE;
		comp1->osc.unison_amount = 3;
		comp1->unison_detune = 0.1;
		preset.components[8].set_component(comp1);
		preset.components[8].bindings.push_back({BindingType::MUL, OSCILATOR_VOLUME_PROPERTY, 0, 0, 1});

		OscilatorComponent* comp2 = new OscilatorComponent();
		comp2->osc.data.waveform = AnalogWaveForm::SAW_DOWN;
		comp2->osc.unison_amount = 1;
		comp2->unison_detune = 0.1;
		preset.components[9].set_component(comp2);
		preset.components[9].bindings.push_back({BindingType::MUL, OSCILATOR_VOLUME_PROPERTY, 0, 1, 0});

		LowPassFilter12Component* filter = new LowPassFilter12Component();
		filter->cutoff = 200;
		preset.components[10].set_component(filter);
		preset.components[10].bindings.push_back({BindingType::ADD, FILTER_INPUT_PROPERTY, 8, -1, 1});
		preset.components[10].bindings.push_back({BindingType::ADD, FILTER_INPUT_PROPERTY, 9, -1, 1});
		preset.components[10].bindings.push_back({BindingType::ADD, FILTER_CUTOFF_PROPERTY, 1, 0, 350});
		preset.components[10].bindings.push_back({BindingType::MUL, FILTER_CUTOFF_PROPERTY, 0, 0.9, 1.1});

		AmpEnvelopeComponent* amp = new AmpEnvelopeComponent();
		amp->envelope = {0.15, 0, 1, 0.5};

		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back({BindingType::ADD, AMP_ENVELOPE_INPUT_PROPERTY, 10, -1, 1});
		preset.components[11].bindings.push_back({BindingType::MUL, AMP_ENVELOPE_AMPLITUDE_PROPERTY, 1, 0.7, 1});
		preset.components[11].audible = true;
	}
		break;
	case 6:
		//Patch 6 -- Bright Lead
	{
		ModEnvelopeComponent *mod = new ModEnvelopeComponent();
		mod->envelope = { 1.0, 1.0, 0.2, 0.3 };
		mod->amplitude = 0.7;
		preset.components[0].set_component(mod);

		ControlChangeComponent *cc = new ControlChangeComponent();
		cc->control = 1;
		preset.components[1].set_component(cc);

		LFOComponent *lfo = new LFOComponent();
		lfo->freq = 6;
		preset.components[2].set_component(lfo);
		preset.components[2].bindings.push_back( { BindingType::MUL,
				LFO_AMPLITUDE_PROPERTY, 1, 0, 1 });

		OscilatorComponent *comp1 = new OscilatorComponent();
		comp1->osc.data.waveform = AnalogWaveForm::SAW_DOWN;
		comp1->osc.unison_amount = 1;
		comp1->unison_detune = 0.1;
		preset.components[8].set_component(comp1);
		preset.components[8].bindings.push_back( { BindingType::MUL,
				OSCILATOR_VOLUME_PROPERTY, 0, 1, 0.5 });
		preset.components[8].bindings.push_back( { BindingType::ADD,
						OSCILATOR_PITCH_PROPERTY, 2, -1, 1 });

		OscilatorComponent *comp2 = new OscilatorComponent();
		comp2->osc.data.waveform = AnalogWaveForm::SAW_DOWN;
		comp2->osc.unison_amount = 1;
		comp2->unison_detune = 0.1;
		comp2->transpose = 2;
		preset.components[9].set_component(comp2);
		preset.components[9].bindings.push_back( { BindingType::MUL,
				OSCILATOR_VOLUME_PROPERTY, 0, 0.5, 1 });
		preset.components[9].bindings.push_back( { BindingType::ADD,
						OSCILATOR_PITCH_PROPERTY, 2, -1, 1 });

		LowPassFilter24Component *filter = new LowPassFilter24Component();
		filter->cutoff = 7800;
		filter->keyboard_tracking = 0.7;
		preset.components[10].set_component(filter);
		preset.components[10].bindings.push_back( { BindingType::ADD,
				FILTER_INPUT_PROPERTY, 8, -1, 1 });
		preset.components[10].bindings.push_back( { BindingType::ADD,
				FILTER_INPUT_PROPERTY, 9, -1, 1 });

		AmpEnvelopeComponent *amp = new AmpEnvelopeComponent();
		amp->envelope = { 0.0005, 0, 1, 0.003 };

		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back( { BindingType::ADD,
				AMP_ENVELOPE_INPUT_PROPERTY, 10, -1, 1 });
		preset.components[11].audible = true;
	}
		break;
	case 7:
		//Patch 7 -- Syncing Sawtooth wave
	{
		ControlChangeComponent *cc = new ControlChangeComponent();
		cc->control = 1;
		preset.components[0].set_component(cc);

		OscilatorComponent *comp1 = new OscilatorComponent();
		comp1->osc.data.waveform = AnalogWaveForm::SAW_DOWN;
		comp1->osc.data.sync = true;
		comp1->osc.data.analog = true;
		preset.components[8].set_component(comp1);
		preset.components[8].bindings.push_back({BindingType::SET, OSCILATOR_SYNC_PROPERTY, 0, 1, 2});

		AmpEnvelopeComponent *amp = new AmpEnvelopeComponent();
		amp->envelope = { 0.0005, 0, 1, 0.003 };

		preset.components[11].set_component(amp);
		preset.components[11].bindings.push_back( { BindingType::ADD,
				AMP_ENVELOPE_INPUT_PROPERTY, 8, -1, 1 });
		preset.components[11].audible = true;
	}
		break;
	}
}

SynthesizerData::SynthesizerData() {
	apply_preset(preset, 1);
}

//Synthesizer
Synthesizer::Synthesizer() {

}

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

void Synthesizer::note_not_pressed(SampleInfo& info, TriggeredNote& note, SoundEngineData& d, size_t note_index) {
	SynthesizerData& data = dynamic_cast<SynthesizerData&>(d);
	for (size_t i = 0; i < data.preset.components.size(); ++i) {
		data.preset.components[i].note_not_pressed(info, note, note_index);
	}
}

void Synthesizer::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& d) {
	SynthesizerData& data = dynamic_cast<SynthesizerData&>(d);
	if (data.update_preset != -1) {
		apply_preset(data.preset, data.update_preset);
		data.update_preset = -1;
	}
}

void Synthesizer::control_change(unsigned int control, unsigned int value, SoundEngineData& d) {
	SynthesizerData& data = dynamic_cast<SynthesizerData&>(d);

	for (size_t i = 0; i < data.preset.components.size(); ++i) {
		data.preset.components[i].control_change(control, value);
	}

	if (control == data.preset.preset_cc) {
		data.update_preset = value/5;
	}
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

