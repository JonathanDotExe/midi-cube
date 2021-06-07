/*
 * organ.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "organ.h"
#include "../oscilator.h"
#include <cmath>

//B3OrganTonewheel
double B3OrganTonewheel::process(SampleInfo& info, double freq, OrganType type) {
	//Rotation
	double volume = this->volume;
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
B3Organ::B3Organ() {
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

void B3Organ::trigger_tonewheel(int tonewheel, double volume, SampleInfo& info, TriggeredNote& note, double compress_volume) {
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
		if (note.pressed || info.time <= note.release_time + tonewheel_data[tonewheel].release_delay) {
			data.tonewheels[tonewheel].volume += volume * vol_mul * fmin(1, (info.time - note.start_time - tonewheel_data[tonewheel].press_delay)/data.preset.click_attack);
		}
		else {
			data.tonewheels[tonewheel].volume += volume * vol_mul * fmax(0, 1 - (info.time - note.release_time - tonewheel_data[tonewheel].release_delay)/data.preset.click_attack);
		}
		data.tonewheels[tonewheel].compress_volume += compress_volume;
	}
}

void B3Organ::process_note_sample(double& lsample, double& rsample, SampleInfo &info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Organ sound
	double drawbar_amount = data.preset.drawbars.size() + (data.preset.percussion_soft ? data.preset.percussion_soft_volume : data.preset.percussion_hard_volume);
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		int tonewheel = note.note + drawbar_notes[i] - ORGAN_LOWEST_TONEWHEEL_NOTE;
		double vol = data.preset.drawbars[i] / (double) ORGAN_DRAWBAR_MAX / drawbar_amount;
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

/*static double calc_vol (size_t keys, double comp_factor) {
	double vol = 1;
	for (size_t i = 2; i <= keys; ++i) {
		vol += comp_factor * 1.0/(i - 1);
	}
	return vol;
}*/

bool B3Organ::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return !note.pressed && info.time > note.release_time + ORGAN_MAX_UP_DELAY + data.preset.click_attack;
};

void B3Organ::process_sample(double& lsample, double& rsample, SampleInfo &info, KeyboardEnvironment& env, EngineStatus& status, Metronome& metronome) {
	//Update properties
	double swell = this->data.swell * SWELL_RANGE + MIN_SWELL;

	//Percussion
	if (status.pressed_notes == 0) {
		data.reset_percussion = true;
	}
	else if (data.reset_percussion) {
		data.percussion_start = info.time + info.time_step;
		data.reset_percussion = false;
	}

	//Play organ sound
	//Compute samples
	double sample = 0;
	double volume = 0;
	for (size_t i = 0; i < data.tonewheels.size(); ++i) {
		volume += data.tonewheels[i].compress_volume;
		sample += data.tonewheels[i].process(info, tonewheel_data[i].freq * env.pitch_bend, data.preset.type) * (1 + (tonewheel_data[i].volume - 1) * data.preset.high_gain_reduction);
	}
	sample *= swell;
	//Compress
	if (volume && data.preset.multi_note_gain != 1) {
		//double v = calc_vol(status.pressed_notes, data.preset.multi_note_gain)/status.pressed_notes;
		sample *= pow(volume, data.preset.multi_note_gain)/volume;
	}

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

	//Amplifier
	lsample = sample;
	rsample = sample;
}

bool B3Organ::control_change(unsigned int control, unsigned int value) {
	bool change = false;
	//Drawbars
	for (size_t i = 0; i < data.preset.drawbar_ccs.size(); ++i) {
		if (data.preset.drawbar_ccs[i] == control) {
			data.preset.drawbars[i] = round((double) value/127 * ORGAN_DRAWBAR_MAX);
			change = true;
		}
	}
	//Percussion
	if (control == data.preset.percussion_cc) {
		data.preset.percussion = value > 0;
		change = true;
	}
	//Percussion Third Harmonic
	if (control == data.preset.percussion_third_harmonic_cc) {
		data.preset.percussion_third_harmonic = value > 0;
		change = true;
	}
	//Percussion Fast Decay
	if (control == data.preset.percussion_fast_decay_cc) {
		data.preset.percussion_fast_decay = value > 0;
		change = true;
	}
	//Percussion Soft
	if (control == data.preset.percussion_soft_cc) {
		data.preset.percussion_soft = value > 0;
		change = true;
	}
	//Vibrato Mix
	if (control == data.preset.vibrato_mix_cc) {
		data.preset.vibrato_mix = value/127.0;
		change = true;
	}
	//Swell
	if (control == data.preset.swell_cc) {
		data.swell = value/127.0;
	}

	return change;
}

template<>
std::string get_engine_name<B3Organ>() {
	return "B3 Organ";
}

void __fix_link_organ_name__ () {
	get_engine_name<B3Organ>();
}

void B3Organ::save_program(EngineProgram **prog) {
	B3OrganProgram* p = dynamic_cast<B3OrganProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new B3OrganProgram();
	}
	p->preset = data.preset;
	*prog = p;
}

void B3Organ::apply_program(EngineProgram *prog) {
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
	//Drawbars
	const auto& drawbars = tree.get_child_optional("drawbars");
	size_t i = 0;
	if (drawbars) {
		for (pt::ptree::value_type& d : drawbars.get()) {
			if (i >= ORGAN_DRAWBAR_COUNT) {
				break;
			}

			preset.drawbars[i] = d.second.get_value<unsigned int>(preset.drawbars[i]);

			++i;
		}
	}
	//Drawbar CCs
	const auto& drawbar_ccs = tree.get_child_optional("drawbar_ccs");
	i = 0;
	if (drawbar_ccs) {
		for (pt::ptree::value_type& d : drawbar_ccs.get()) {
			if (i >= ORGAN_DRAWBAR_COUNT) {
				break;
			}

			preset.drawbar_ccs[i] = d.second.get_value<unsigned int>(preset.drawbar_ccs[i]);
			++i;
		}
	}

	//Modeling
	preset.harmonic_foldback_volume = tree.get<double>("harmonic_foldback_volume", 1);
	preset.multi_note_gain = tree.get<double>("multi_note_gain", 0.8);
	preset.click_attack = tree.get<double>("click_attack", 0.00004);
	preset.high_gain_reduction = tree.get<double>("high_gain_reduction", 0.5);

	//Percussion
	preset.percussion = tree.get<bool>("percussion", false);
	preset.percussion_third_harmonic = tree.get<bool>("percussion_third_harmonic", true);
	preset.percussion_soft = tree.get<bool>("percussion_soft", true);
	preset.percussion_fast_decay = tree.get<bool>("percussion_fast_decay", true);

	preset.percussion_cc = tree.get<unsigned int>("percussion_cc", 24);
	preset.percussion_third_harmonic_cc = tree.get<unsigned int>("percussion_third_harmonic_cc", 25);
	preset.percussion_soft_cc = tree.get<unsigned int>("percussion_soft_cc", 26);
	preset.percussion_fast_decay_cc = tree.get<unsigned int>("percussion_fast_decay_cc", 27);

	//Vibrato/Chorus
	preset.vibrato_mix = tree.get<double>("vibrato_mix", 0.0);
	preset.vibrato_mix_cc = tree.get<unsigned int>("vibrato_mix_cc", 38);
	preset.swell_cc = tree.get<unsigned int>("swell_cc", 11);
}

boost::property_tree::ptree B3OrganProgram::save() {
	boost::property_tree::ptree tree;

	//Drawbars
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		tree.add("drawbars.drawbar", preset.drawbars[i]);
		tree.add("drawbar_ccs.drawbar", preset.drawbar_ccs[i]);
	}

	//Modeling
	tree.put("harmonic_foldback_volume", preset.harmonic_foldback_volume);
	tree.put("multi_note_gain", preset.multi_note_gain);
	tree.put("click_attack", preset.click_attack);
	tree.put("high_gain_reduction", preset.high_gain_reduction);

	//Percussion
	tree.put("percussion", preset.percussion);
	tree.put("percussion_third_harmonic", preset.percussion_third_harmonic);
	tree.put("percussion_soft", preset.percussion_soft);
	tree.put("percussion_fast_decay", preset.percussion_fast_decay);

	tree.put("percussion_cc", preset.percussion_cc);
	tree.put("percussion_third_harmonic_cc", preset.percussion_third_harmonic_cc);
	tree.put("percussion_soft_cc", preset.percussion_soft_cc);
	tree.put("percussion_fast_decay_cc", preset.percussion_fast_decay_cc);

	//Vibrato/Chorus
	tree.put("vibrato_mix", preset.vibrato_mix);
	tree.put("vibrato_mix_cc", preset.vibrato_mix_cc);
	tree.put("swell_cc", preset.swell_cc);


	return tree;
}
