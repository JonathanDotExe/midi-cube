/*
 * organ.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "organ.h"
#include <cmath>

//B3OrganTonewheel
bool B3OrganTonewheel::has_turned_since(double time) {
	return time <= last_turn;
}

double B3OrganTonewheel::process(SampleInfo& info, double freq) {
	double volume = curr_vol + dynamic_vol;
	//Rotation
	double old_rot = rotation;
	rotation += freq * info.time_step;
	if ((int) (old_rot/0.5) != (int)(rotation/0.5)) {
		last_turn = info.time;
		curr_vol = static_vol;
	}
	//Reset
	static_vol = 0;
	dynamic_vol = 0;
	//Signal
	if (volume) {
		return sin(freq_to_radians(rotation)) * volume;
	}
	else {
		return 0;
	}
}


//B3Organ
B3Organ::B3Organ() {
	drawbar_harmonics = { 0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8 };
	drawbar_notes = {-12, 7, 0, 12, 19, 24, 28, 31, 36};
	foldback_freq = note_to_freq(ORGAN_FOLDBACK_NOTE);

	for (size_t i = 0; i < tonewheel_frequencies.size(); ++i) {
		tonewheel_frequencies[i] = note_to_freq(ORGAN_LOWEST_TONEWHEEL_NOTE + i);
	}

	for (cutoff_tonewheel = 0; cutoff_tonewheel < tonewheel_frequencies.size(); ++cutoff_tonewheel) {
		if (tonewheel_frequencies[cutoff_tonewheel] > ROTARY_CUTOFF) {
			break;
		}
	}
}

static inline double sound_delay(double rotation, double radius, unsigned int sample_rate) {
	/*double dst =
			rotation >= 0 ?
					(SPEAKER_RADIUS - radius * rotation) :
					(SPEAKER_RADIUS + radius * rotation + radius * 2);
	return round(dst / SOUND_SPEED * sample_rate);*/
	return (1 + rotation) * 0.0005 * sample_rate;
}

void B3Organ::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& d, size_t note_index) {
	B3OrganData& data = dynamic_cast<B3OrganData&>(d);

	//Organ sound
	for (size_t i = 0; i < data.preset.drawbars.size(); ++i) {
		int tonewheel = note.note + drawbar_notes.at(i) - ORGAN_LOWEST_TONEWHEEL_NOTE;

		while (tonewheel >= (int) data.tonewheels.size()) {
			tonewheel -= 12;
		}
		if (tonewheel >= 0) {
			data.tonewheels[tonewheel].static_vol += data.preset.drawbars[i] / (double) ORGAN_DRAWBAR_MAX / data.preset.drawbars.size();
		}
	}
}

void B3Organ::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, SoundEngineData& d) {
	B3OrganData& data = dynamic_cast<B3OrganData&>(d);

	//Play organ sound
	if (data.preset.rotary) {
		double horn_sample = 0;
		double bass_sample = 0;

		//Compute samples
		size_t i = 0;
		for (; i < cutoff_tonewheel && i < data.tonewheels.size(); ++i) {
			bass_sample += data.tonewheels[i].process(info, tonewheel_frequencies[i]); //TODO pitch bend
		}
		for (; i < data.tonewheels.size(); ++i) {
			horn_sample += data.tonewheels[i].process(info, tonewheel_frequencies[i]); //TODO pitch bend
		}

		//Horn
		double horn_rot = sin(freq_to_radians(data.horn_rotation));
		double horn_pitch_rot = cos(freq_to_radians(data.horn_rotation));
		double lhorn_delay = sound_delay(horn_pitch_rot, HORN_RADIUS, info.sample_rate);
		double rhorn_delay = sound_delay(-horn_pitch_rot, HORN_RADIUS, info.sample_rate);
		//Bass
		double bass_rot = sin(freq_to_radians(data.bass_rotation));
		double bass_pitch_rot = cos(freq_to_radians(data.bass_rotation));
		double lbass_delay = sound_delay(bass_pitch_rot, BASS_RADIUS, info.sample_rate);
		double rbass_delay = sound_delay(-bass_pitch_rot, BASS_RADIUS, info.sample_rate);

		//Process
		data.left_del.add_isample(horn_sample * (0.5 + horn_rot * 0.5), lhorn_delay);
		data.left_del.add_isample(bass_sample * (0.5 + bass_rot * 0.5), lbass_delay);
		data.right_del.add_isample(horn_sample * (0.5 - horn_rot * 0.5), rhorn_delay);
		data.right_del.add_isample(bass_sample * (0.5 - bass_rot * 0.5), rbass_delay);
	}
	else {
		//Compute samples
		double sample = 0;
		for (size_t i = 0; i < data.tonewheels.size(); ++i) {
			sample += data.tonewheels[i].process(info, tonewheel_frequencies[i]); //TODO pitch bend
		}
		//Play
		for (size_t i = 0; i < channels.size() ; ++i) {
			channels[i] += sample;
		}
	}

	//Switch speaker speed
	if (data.curr_rotary_fast != data.preset.rotary_fast) {
		data.curr_rotary_fast = data.preset.rotary_fast;
		if (data.curr_rotary_fast) {
			data.horn_speed.set(ROTARY_HORN_FAST_FREQUENCY, info.time, ROTARY_HORN_FAST_RAMP);
			data.bass_speed.set(ROTARY_BASS_FAST_FREQUENCY, info.time, ROTARY_BASS_FAST_RAMP);
		}
		else {
			data.horn_speed.set(ROTARY_HORN_SLOW_FREQUENCY, info.time, ROTARY_HORN_SLOW_RAMP);
			data.bass_speed.set(ROTARY_BASS_SLOW_FREQUENCY, info.time, ROTARY_BASS_SLOW_RAMP);
		}
	}

	//Rotate speakers
	data.horn_rotation += data.horn_speed.get(info.time) * info.time_step;
	data.bass_rotation -= data.bass_speed.get(info.time) * info.time_step;

	//Play delay
	double left = (data.left_del.process());
	double right = (data.right_del.process());

	for (size_t i = 0; i < channels.size(); ++i) {
		if (i % 2 == 0) {
			channels[i] += left + right * 0.7;
		}
		else {
			channels[i] += right + left * 0.7;
		}
	}
}

void B3Organ::control_change(unsigned int control, unsigned int value, SoundEngineData& d) {
	B3OrganData& data = dynamic_cast<B3OrganData&>(d);
	//Drawbars
	for (size_t i = 0; i < data.preset.drawbar_ccs.size(); ++i) {
		if (data.preset.drawbar_ccs[i] == control) {
			data.preset.drawbars[i] = round((double) value/127 * ORGAN_DRAWBAR_MAX);
		}
	}
	//Rotary
	if (control == data.preset.rotary_cc) {
		data.preset.rotary = value > 0;
	}
	if (control == data.preset.rotary_speed_cc) {
		data.preset.rotary_fast = value > 0;
	}
}

SoundEngineData* B3Organ::create_data() {
	return new B3OrganData();
}

std::string B3Organ::get_name() {
	return "B3 Organ";
}

