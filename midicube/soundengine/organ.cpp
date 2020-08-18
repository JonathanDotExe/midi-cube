/*
 * organ.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "organ.h"
#include <cmath>

//B3Organ
B3Organ::B3Organ() {
	drawbar_harmonics = { 0.5, 0.5 * 3, 1, 2, 3, 4, 5, 6, 8 };
}

static inline unsigned int sound_delay(double rotation, double radius, unsigned int sample_rate) {
	/*double dst =
			rotation >= 0 ?
					(SPEAKER_RADIUS - radius * rotation) :
					(SPEAKER_RADIUS + radius * rotation + radius * 2);
	return round(dst / SOUND_SPEED * sample_rate);*/
	return round((1 + rotation) * radius / SOUND_SPEED * sample_rate);
}

void B3Organ::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& d, size_t note_index) {
	B3OrganData& data = dynamic_cast<B3OrganData&>(d);

	double horn_sample = 0;
	double bass_sample = 0;

	double time = info.time + note.phase_shift;

	//Organ sound
	for (size_t i = 0; i < data.preset.drawbars.size(); ++i) {
		double f = note.freq * drawbar_harmonics[i];
		while (f > 5593) {
			f /= 2.0;
		}
		/*double samp_off = f/ROTARY_CUTOFF;
		if (f > ROTARY_CUTOFF) {
			double sample = data.drawbars[i] / 8.0 * sine_wave(time, f) / data.drawbars.size();
			horn_sample += sample;
			//Emulate filter
			double amp = db_to_amp(1/samp_off * -12);
			bass_sample += amp * sample;
		} else {
			double sample = data.drawbars[i] / 8.0 * sine_wave(time, f) / data.drawbars.size();
			bass_sample += sample;
			//Emulate filter
			double amp = db_to_amp(samp_off/2 * -12);
			horn_sample += amp * sample;
		}*/

		if (f > ROTARY_CUTOFF) {
			horn_sample += data.preset.drawbars[i] / (double) ORGAN_DRAWBAR_MAX * sine_wave(time, f) / data.preset.drawbars.size();
		} else {
			bass_sample += data.preset.drawbars[i] / (double) ORGAN_DRAWBAR_MAX * sine_wave(time, f) / data.preset.drawbars.size();
		}
	}
	double sample = 0;

	//Rotary speaker
	if (data.preset.rotary) {
		double horn_speed = data.preset.rotary_fast ? ROTARY_HORN_FAST_FREQUENCY : ROTARY_HORN_SLOW_FREQUENCY;
		double bass_speed = data.preset.rotary_fast ? ROTARY_BASS_FAST_FREQUENCY : ROTARY_BASS_SLOW_FREQUENCY;

		//Horn
		double horn_rot = sine_wave(info.time, horn_speed);
		unsigned int lhorn_delay = sound_delay(horn_rot, HORN_RADIUS, info.sample_rate);
		unsigned int rhorn_delay = sound_delay(-horn_rot, HORN_RADIUS, info.sample_rate);
		//Bass
		double bass_rot = sine_wave(info.time, bass_speed);
		unsigned int lbass_delay = sound_delay(bass_rot, BASS_RADIUS, info.sample_rate);
		unsigned int rbass_delay = sound_delay(-bass_rot, BASS_RADIUS, info.sample_rate);

		//Process
		data.left_horn_del.add_sample(horn_sample * (0.5 + horn_rot * 0.5), lhorn_delay);
		data.left_bass_del.add_sample(bass_sample * (0.5 + bass_rot * 0.5), lbass_delay);
		data.right_horn_del.add_sample(horn_sample * (0.5 - horn_rot * 0.5), rhorn_delay);
		data.right_bass_del.add_sample(bass_sample * (0.5 - bass_rot * 0.5), rbass_delay);
	} else {
		sample += horn_sample + bass_sample;
		for (size_t i = 0; i < channels.size() ; ++i) {
			channels[i] += sample;
		}
	}
}

void B3Organ::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, SoundEngineData& d) {
	B3OrganData& data = dynamic_cast<B3OrganData&>(d);
	double left = (data.left_horn_del.process() + data.left_bass_del.process());
	double right = (data.right_horn_del.process() + data.right_bass_del.process());

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

