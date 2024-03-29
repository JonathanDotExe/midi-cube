/*
 * organ.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "organ.h"
#include "../../framework/dsp/oscilator.h"
#include "../view/B3OrganView.h"
#include <cmath>

//B3OrganTonewheel
double B3OrganTonewheel::process(const SampleInfo& info, double freq, OrganType type, double vol_mul, double click_attack, double click_release) {
	//Rotation
	this->volume_buffer.set(this->volume * vol_mul, 1/click_attack,1/click_release);
	double volume = this->volume_buffer.process(info.time_step);
	rotation += freq * info.time_step;
	this->volume = 0;
	this->compress_volume = 0;
	//Signal
	if (volume) {
		if (type == ORGAN_TYPE_B3) {
			return sin(freq_to_radians(rotation)) * volume;
		}
		else {
			double phase = fmod(rotation, 1.0);
			double pphase = fmod(rotation + 0.5, 1.0);
			double step = info.time_step * freq;
			return (triangle_wave(phase) + integrated_polyblep(phase, step) * 4 * step - integrated_polyblep(pphase, step) * 4 * step) * volume;
		}
	}
	else {
		return 0;
	}
}

//B3Organ
B3Organ::B3Organ(PluginHost& h, Plugin& p) : SoundEngine(h, p) {
	//Bind
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		binder.add_binding(&data.preset.upper_drawbars[i]);
	}
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		binder.add_binding(&data.preset.lower_drawbars[i]);
	}
	for (size_t i = 0; i < ORGAN_BASS_DRAWBAR_COUNT; ++i) {
		binder.add_binding(&data.preset.bass_drawbars[i]);
	}
	binder.add_binding(&data.preset.percussion);
	binder.add_binding(&data.preset.percussion_third_harmonic);
	binder.add_binding(&data.preset.percussion_soft);
	binder.add_binding(&data.preset.percussion_fast_decay);

	binder.add_binding(&data.preset.vibrato_mix);
	binder.add_binding(&data.preset.swell);

	drawbar_notes = {-12, 7, 0, 12, 19, 24, 28, 31, 36};
	std::vector<double> gear_ratios = {
		0.817307692,
		0.865853659,
		0.917808219,
		0.972222222,
		1.030000000,
		1.090909091,
		1.156250000,
		1.225000000,
		1.297297297,
		1.375000000,
		1.456521739,
		1.542857143
	};

	//Frequencies
	int teeth = 1;
	for (size_t i = 0; i < tonewheel_data.size(); ++i) {
		if (i % gear_ratios.size() == 0) {
			teeth *= 2;
			if (teeth > 192) {
				teeth = 192;
			}
		}
		tonewheel_data[i].freq = 20 * teeth * gear_ratios[(i + (teeth == 192 ? 5 : 0)) % gear_ratios.size()];
	}
	//Press/Release delays
	srand(888800000);
	for (size_t i = 0; i < tonewheel_data.size(); ++i) {
		tonewheel_data[i].press_delay = (double) rand()/RAND_MAX * ORGAN_MAX_DOWN_DELAY;
	}
	for (size_t i = 0; i < tonewheel_data.size(); ++i) {
		tonewheel_data[i].release_delay = (double) rand()/RAND_MAX * ORGAN_MAX_UP_DELAY;
	}
	//Tonewheel volumes
	const size_t start_tw = 72;
	for (size_t i = 0; i < tonewheel_data.size() - start_tw; ++i) {
		double curr_vol = db_to_amp((-1.5 * ((int) i/12 + 1)));
		tonewheel_data[i + start_tw].volume = curr_vol;
	}
}

void B3Organ::trigger_tonewheel(int tonewheel, double volume, const SampleInfo& info, Voice& note, double compress_volume) {
	bool vol_mul = 1;
	while (tonewheel < 0) {
		tonewheel += 12;
		vol_mul = data.preset.harmonic_foldback_volume;
	}
	while (tonewheel >= (int) data.tonewheels.size()) {
		tonewheel -= 12;
		vol_mul = data.preset.harmonic_foldback_volume;
	}
	if (tonewheel >= 0 &&
			info.time >= note.start_time + tonewheel_data[tonewheel].press_delay) {
		if (note.state == VOICE_PRESSED || info.time <= note.release_time + tonewheel_data[tonewheel].release_delay) {
			data.tonewheels[tonewheel].volume += volume * vol_mul;
		}
		else {
			data.tonewheels[tonewheel].volume += volume * vol_mul;
		}
		data.tonewheels[tonewheel].compress_volume += compress_volume;
	}
}

void B3Organ::process_note_sample(const SampleInfo &info, Voice& note, size_t note_index) {
	//Organ sound
	switch (note.channel) {
	case 0:
	{
		//Upper manual
		double drawbar_amount = ORGAN_DRAWBAR_COUNT + ((!data.preset.percussion_soft && data.preset.percussion) ? data.preset.percussion_hard_volume : data.preset.percussion_soft_volume);
		for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
			int tonewheel = note.note + drawbar_notes[i] - ORGAN_LOWEST_TONEWHEEL_NOTE;
			double vol = data.preset.upper_drawbars[i] / (double) ORGAN_DRAWBAR_MAX / drawbar_amount;
			trigger_tonewheel(tonewheel, vol, info, note, vol);
		}

		//Percussion
		double decay = data.preset.percussion_fast_decay ? data.preset.percussion_fast_decay_time : data.preset.percussion_slow_decay_time;
		if (data.preset.percussion && info.time - data.percussion_start <= decay) {
			double vol = (1 - (info.time - data.percussion_start)/decay) * (data.preset.percussion_soft ? data.preset.percussion_soft_volume : data.preset.percussion_hard_volume);
			vol /= drawbar_amount;
			int tonewheel = note.note + (data.preset.percussion_third_harmonic ? 19 : 12) - ORGAN_LOWEST_TONEWHEEL_NOTE;

			trigger_tonewheel(tonewheel, vol, info, note, 0);
		}
	}
		break;
	case 1:
	{
		//Lower manual
		double drawbar_amount = ORGAN_DRAWBAR_COUNT + (data.preset.percussion_soft_volume);
		for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
			int tonewheel = note.note + drawbar_notes[i] - ORGAN_LOWEST_TONEWHEEL_NOTE;
			double vol = data.preset.lower_drawbars[i] / (double) ORGAN_DRAWBAR_MAX / drawbar_amount;
			trigger_tonewheel(tonewheel, vol, info, note, vol);
		}
	}
		break;
	case 2:
	{
		//Bass manual
		double drawbar_amount = ORGAN_BASS_DRAWBAR_COUNT + (data.preset.percussion_soft_volume);
		for (size_t i = 0; i < ORGAN_BASS_DRAWBAR_COUNT; ++i) {
			int tonewheel = note.note + drawbar_notes[i * 2] - ORGAN_LOWEST_TONEWHEEL_NOTE; //FIXME possible overflow when sizes change
			double vol = data.preset.bass_drawbars[i] / (double) ORGAN_DRAWBAR_MAX / drawbar_amount;
			trigger_tonewheel(tonewheel, vol, info, note, vol);
		}
	}
		break;
	}
}

/*static double calc_vol (size_t keys, double comp_factor) {
	double vol = 1;
	for (size_t i = 2; i <= keys; ++i) {
		vol += comp_factor * 1.0/(i - 1);
	}
	return vol;
}*/

bool B3Organ::note_finished(const SampleInfo& info, Voice& note, size_t note_index) {
	return (note.state != VOICE_PRESSED || (host_environment.sustain && note.release_time >= host_environment.sustain_time)) && info.time > note.release_time + ORGAN_MAX_UP_DELAY + data.preset.click_attack;
};

void B3Organ::process_sample(const SampleInfo &info) {
	//Update properties
	double swell = SWELL_MIN + pow(data.preset.swell, 1.2) * SWELL_RANGE;
	data.swell_port.set(swell, 1/0.1, 1/0.1);
	swell = data.swell_port.process(info.time_step);

	//Percussion
	bool pressed = false;
	for (size_t i = 0; i < B3_ORGAN_POLYPHONY && !pressed; ++i) {
		if (voice_mgr.note[i].channel == 0 && voice_mgr.note[i].state != VOICE_INACTIVE) {
			pressed = true;
		}
	}
	if (!pressed) {
		data.reset_percussion = true;
	}
	else if (data.reset_percussion) {
		data.percussion_start = info.time + info.time_step;
		data.reset_percussion = false;
	}

	//Play organ sound
	//Compress
	double sample = 0;
	double volume = 0;
	for (size_t i = 0; i < data.tonewheels.size(); ++i) {
		volume += data.tonewheels[i].compress_volume;
	}
	double vol_mul = 1;
	if (volume && data.preset.multi_note_gain != 1) {
		vol_mul = pow(volume, data.preset.multi_note_gain)/volume;
	}
	//Compute samples
	for (size_t i = 0; i < data.tonewheels.size(); ++i) {
		sample += data.tonewheels[i].process(info, tonewheel_data[i].freq * host_environment.pitch_bend, data.preset.type, vol_mul, data.preset.click_attack, data.preset.click_release) * (1 + (tonewheel_data[i].volume - 1) * data.preset.high_gain_reduction);
	}
	sample *= swell;

	//Chorus/Vibrato
	if (data.preset.vibrato_mix) {
		size_t delays = data.delays.size();
		//Get vibrato signal
		double vibrato = 0;
		double scanner_pos = data.scanner_phase * (delays - 1);
		size_t scanner_pos_1 = floor(scanner_pos);
		size_t scanner_pos_2 = ceil(scanner_pos);
		double prog = scanner_pos - scanner_pos_1;

		for (size_t i = 0; i < delays; ++i) {
			data.delays[i].add_isample(sample, (int) (0.0001 * info.sample_rate * (i + 1)));
			if (i == scanner_pos_1) {
				vibrato += data.delays[i].process() * (1 - prog);
			}
			else if (i == scanner_pos_2) {
				vibrato += data.delays[i].process() * prog;
			}
			else {
				data.delays[i].process();
			}
		}
		//Move scanner
		data.scanner_phase += (data.scanner_inverse ? (-1) : (1)) * info.time_step * ORGAN_VIBRATO_RATE * 2;
		if (data.scanner_phase > 1) {
			data.scanner_phase = 1 - fmod(data.scanner_phase, 1.0);
			data.scanner_inverse = true;
		}
		else if (data.scanner_phase < 0) {
			data.scanner_phase = fmod(-data.scanner_phase, 1.0);
			data.scanner_inverse = false;
		}
		//Play back
		sample = sample * (1 - fmax(0, data.preset.vibrato_mix - 0.5) * 2) + vibrato * (fmin(1, data.preset.vibrato_mix * 2));
	}

	//Play back
	outputs[0] = sample;
	outputs[1] = sample;
}

void B3Organ::control_change(unsigned int control, unsigned int value) {

}

void B3Organ::save_program(PluginProgram **prog) {
	B3OrganProgram* p = dynamic_cast<B3OrganProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new B3OrganProgram();
	}
	p->preset = data.preset;
	*prog = p;
}

void B3Organ::apply_program(PluginProgram *prog) {
	B3OrganProgram* p = dynamic_cast<B3OrganProgram*>(prog);
	if (p) {
		data.preset = p->preset;
	}
	else {
		data.preset = {};
	}
}

void B3OrganProgram::load(boost::property_tree::ptree tree) {
	preset = {};
	//Upper Drawbars
	const auto& upper_drawbars = tree.get_child_optional("upper_drawbars");
	size_t i = 0;
	if (upper_drawbars) {
		for (pt::ptree::value_type& d : upper_drawbars.get()) {
			if (i >= ORGAN_DRAWBAR_COUNT) {
				break;
			}

			preset.upper_drawbars[i].load(d.second);

			++i;
		}
	}
	else {
		const auto& drawbars = tree.get_child_optional("drawbars");
		i = 0;
		if (drawbars) {
			for (pt::ptree::value_type& d : drawbars.get()) {
				if (i >= ORGAN_DRAWBAR_COUNT) {
					break;
				}

				preset.upper_drawbars[i].load(d.second);

				++i;
			}
		}
	}

	//Lower drawbars
	const auto& lower_drawbars = tree.get_child_optional("lower_drawbars");
	i = 0;
	if (lower_drawbars) {
		for (pt::ptree::value_type& d : lower_drawbars.get()) {
			if (i >= ORGAN_DRAWBAR_COUNT) {
				break;
			}

			preset.lower_drawbars[i].load(d.second);

			++i;
		}
	}

	//Bass drawbars
	const auto& bass_drawbars = tree.get_child_optional("bass_drawbars");
	i = 0;
	if (bass_drawbars) {
		for (pt::ptree::value_type& d : bass_drawbars.get()) {
			if (i >= ORGAN_BASS_DRAWBAR_COUNT) {
				break;
			}

			preset.lower_drawbars[i].load(d.second);

			++i;
		}
	}

	//Modeling
	preset.harmonic_foldback_volume = tree.get<double>("harmonic_foldback_volume", 1);
	preset.multi_note_gain = tree.get<double>("multi_note_gain", 0.8);
	preset.click_attack = tree.get<double>("click_attack", 0.00015);
	preset.click_release = tree.get<double>("click_release", 0.0005);
	preset.high_gain_reduction = tree.get<double>("high_gain_reduction", 0.5);

	//Percussion
	preset.percussion.load(tree, "percussion", false);
	preset.percussion_third_harmonic.load(tree, "percussion_third_harmonic", true);
	preset.percussion_soft.load(tree, "percussion_soft", true);
	preset.percussion_fast_decay.load(tree, "percussion_fast_decay", true);

	//Vibrato/Chorus
	preset.vibrato_mix.load(tree, "vibrato_mix", 0.0);
	preset.swell.load(tree, "swell", 1);
}

boost::property_tree::ptree B3OrganProgram::save() {
	boost::property_tree::ptree tree;

	//Drawbars
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		preset.upper_drawbars[i].save(tree, "upper_drawbars.drawbar");
	}
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		preset.lower_drawbars[i].save(tree, "lower_drawbars.drawbar");
	}
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		preset.bass_drawbars[i].save(tree, "bass_drawbars.drawbar");
	}

	//Modeling
	tree.put("harmonic_foldback_volume", preset.harmonic_foldback_volume);
	tree.put("multi_note_gain", preset.multi_note_gain);
	tree.put("click_attack", preset.click_attack);
	tree.put("click_release", preset.click_release);
	tree.put("high_gain_reduction", preset.high_gain_reduction);

	//Percussion
	preset.percussion.save(tree, "percussion");
	preset.percussion_third_harmonic.save(tree, "percussion_third_harmonic");
	preset.percussion_soft.save(tree, "percussion_soft");
	preset.percussion_fast_decay.save(tree, "percussion_fast_decay");
	//Vibrato/Chorus
	preset.vibrato_mix.save(tree, "vibrato_mix");
	preset.swell.save(tree, "swell");


	return tree;
}

std::string B3OrganProgram::get_plugin_name() {
	return B3_ORGAN_IDENTIFIER;
}

PluginProgram* OrganPlugin::create_program() {
	return new B3OrganProgram();
}

PluginInstance* OrganPlugin::create(PluginHost *host) {
	return new B3Organ(*host, *this);
}

Menu* B3Organ::create_menu() {
	return create_menu(0);
}

Menu* B3Organ::create_menu(unsigned int channel) {
	return new FunctionMenu([this, channel]() { return new B3OrganView(*this, channel); }, [this]() {
			ControlView* view = new ControlView("B3 Organ");
			for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
				view->bind(&data.preset.upper_drawbars[i], ControlType::SLIDER, i, 0);
			}
			for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
				view->bind(&data.preset.lower_drawbars[i], ControlType::SLIDER, i, 1);
			}
			view->bind(&data.preset.percussion, ControlType::BUTTON, 2, 0);
			view->bind(&data.preset.percussion_fast_decay, ControlType::BUTTON, 3, 0);
			view->bind(&data.preset.percussion_soft, ControlType::BUTTON, 4, 0);
			view->bind(&data.preset.percussion_third_harmonic, ControlType::BUTTON, 4, 0);

			view->bind(&data.preset.vibrato_mix, ControlType::KNOB, 1, 0);
			view->init(this);
			return view;
		});
}

void B3Organ::init() {
	binder.init(host.get_binding_handler(), this);
}

void B3Organ::cleanup() {
	binder.unbind();
}
