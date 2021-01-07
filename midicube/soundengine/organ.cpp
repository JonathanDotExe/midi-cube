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

	int teeth = 1;
	for (size_t i = 0; i < tonewheel_frequencies.size(); ++i) {
		if (i % gear_ratios.size() == 0) {
			teeth *= 2;
			if (teeth > 192) {
				teeth = 192;
			}
		}
		tonewheel_frequencies[i] = 20 * teeth * gear_ratios[i % gear_ratios.size()];
	}
}

void B3Organ::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Organ sound
	for (size_t i = 0; i < data.preset.drawbars.size(); ++i) {
		int tonewheel = note.note + drawbar_notes.at(i) - ORGAN_LOWEST_TONEWHEEL_NOTE;
		double volume = 1;

		while (tonewheel >= (int) data.tonewheels.size()) {
			tonewheel -= 12;
			volume *= data.preset.harmonic_foldback_volume;
		}
		if (tonewheel >= 0) {
			data.tonewheels[tonewheel].static_vol += data.preset.drawbars[i] / (double) ORGAN_DRAWBAR_MAX / data.preset.drawbars.size() * volume;
		}
	}
	//Percussion
	double decay = data.preset.percussion_fast_decay ? data.preset.percussion_fast_decay_time : data.preset.percussion_slow_decay_time;
	if (data.preset.percussion && info.time - data.percussion_start <= decay) {
		double vol = (1 - (info.time - data.percussion_start)/decay) * (data.preset.percussion_soft ? data.preset.percussion_soft_volume : data.preset.percussion_hard_volume);
		vol *= 1.0/ORGAN_DRAWBAR_COUNT * 2;
		int tonewheel = note.note + (data.preset.percussion_third_harmonic ? 19 : 12);

		while (tonewheel >= (int) data.tonewheels.size()) {
			tonewheel -= 12;
			vol *= data.preset.harmonic_foldback_volume;
		}
		if (tonewheel >= 0 /*&& data.tonewheels[tonewheel].has_turned_since(data.percussion_start)*/) {
			data.tonewheels[tonewheel].static_vol += vol;
		}
	}
}

static double calc_vol (size_t keys, double comp_factor) {
	double vol = 1;
	for (size_t i = 2; i <= keys; ++i) {
		vol += comp_factor * 1.0/(i - 1);
	}
	return vol;
}

void B3Organ::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo &info, KeyboardEnvironment& env, EngineStatus& status) {
	//Update properties
	if (update_request) {
		submit_change(B3OrganProperty::pB3Drawbar1,
				(int) data.preset.drawbars[0]);
		submit_change(B3OrganProperty::pB3Drawbar2,
				(int) data.preset.drawbars[1]);
		submit_change(B3OrganProperty::pB3Drawbar3,
				(int) data.preset.drawbars[2]);
		submit_change(B3OrganProperty::pB3Drawbar4,
				(int) data.preset.drawbars[3]);
		submit_change(B3OrganProperty::pB3Drawbar5,
				(int) data.preset.drawbars[4]);
		submit_change(B3OrganProperty::pB3Drawbar6,
				(int) data.preset.drawbars[5]);
		submit_change(B3OrganProperty::pB3Drawbar7,
				(int) data.preset.drawbars[6]);
		submit_change(B3OrganProperty::pB3Drawbar8,
				(int) data.preset.drawbars[7]);
		submit_change(B3OrganProperty::pB3Drawbar9,
				(int) data.preset.drawbars[8]);
		submit_change(B3OrganProperty::pB3DrawbarCC1,
				(int) data.preset.drawbar_ccs[0]);
		submit_change(B3OrganProperty::pB3DrawbarCC2,
				(int) data.preset.drawbar_ccs[1]);
		submit_change(B3OrganProperty::pB3DrawbarCC3,
				(int) data.preset.drawbar_ccs[2]);
		submit_change(B3OrganProperty::pB3DrawbarCC4,
				(int) data.preset.drawbar_ccs[3]);
		submit_change(B3OrganProperty::pB3DrawbarCC5,
				(int) data.preset.drawbar_ccs[4]);
		submit_change(B3OrganProperty::pB3DrawbarCC6,
				(int) data.preset.drawbar_ccs[5]);
		submit_change(B3OrganProperty::pB3DrawbarCC7,
				(int) data.preset.drawbar_ccs[6]);
		submit_change(B3OrganProperty::pB3DrawbarCC8,
				(int) data.preset.drawbar_ccs[7]);
		submit_change(B3OrganProperty::pB3DrawbarCC9,
				(int) data.preset.drawbar_ccs[8]);
		submit_change(B3OrganProperty::pB3HarmonicFoldbackVolume,
				data.preset.harmonic_foldback_volume);
		submit_change(B3OrganProperty::pB3DistortionType,
				data.preset.distortion_type);
		submit_change(B3OrganProperty::pB3NormalizeOverdrive,
				data.preset.normalize_overdrive);
		submit_change(B3OrganProperty::pB3Overdrive, data.preset.overdrive);
		submit_change(B3OrganProperty::pB3OverdriveCC,
				(int) data.preset.overdrive_cc);
		submit_change(B3OrganProperty::pB3MultiNoteGain,
				data.preset.multi_note_gain);
		submit_change(B3OrganProperty::pB3Rotary, data.preset.rotary.on);
		submit_change(B3OrganProperty::pB3RotarySpeed, data.preset.rotary.fast);
		submit_change(B3OrganProperty::pB3RotaryCC,
				(int) data.preset.rotary_cc);
		submit_change(B3OrganProperty::pB3RotarySpeedCC,
				(int) data.preset.rotary_speed_cc);
		submit_change(B3OrganProperty::pB3RotaryStereoMix,
				data.preset.rotary.stereo_mix);
		submit_change(B3OrganProperty::pB3RotaryGain, data.preset.rotary.gain);
		submit_change(B3OrganProperty::pB3RotaryType, data.preset.rotary.type);
		submit_change(B3OrganProperty::pB3RotaryDelay,
				data.preset.rotary.max_delay);
		submit_change(B3OrganProperty::pB3Percussion, data.preset.percussion);
		submit_change(B3OrganProperty::pB3PercussionThirdHarmonic,
				data.preset.percussion_third_harmonic);
		submit_change(B3OrganProperty::pB3PercussionSoft,
				data.preset.percussion_soft);
		submit_change(B3OrganProperty::pB3PercussionFastDecay,
				data.preset.percussion_fast_decay);
		submit_change(B3OrganProperty::pB3PercussionSoftVolume,
				data.preset.percussion_soft_volume);
		submit_change(B3OrganProperty::pB3PercussionHardVolume,
				data.preset.percussion_hard_volume);
		submit_change(B3OrganProperty::pB3PercussionFastDecayTime,
				data.preset.percussion_fast_decay_time);
		submit_change(B3OrganProperty::pB3PercussionSlowDecayTime,
				data.preset.percussion_slow_decay_time);
		submit_change(B3OrganProperty::pB3PercussionCC,
				(int) data.preset.percussion_cc);
		submit_change(B3OrganProperty::pB3PercussionThirdHarmonicCC,
				(int) data.preset.percussion_third_harmonic_cc);
		submit_change(B3OrganProperty::pB3PercussionSoftCC,
				(int) data.preset.percussion_soft_cc);
		submit_change(B3OrganProperty::pB3PercussionFastDecayCC,
				(int) data.preset.percussion_fast_decay_cc);
		submit_change(B3OrganProperty::pB3SwellCC,
				(int) data.preset.swell_cc);
		update_request = false;
	}

	double swell = this->data.swell * SWELL_RANGE + MIN_SWELL;

	//Play organ sound
	//Compute samples
	double sample = 0;
	double vol = 0;

	for (size_t i = 0; i < data.tonewheels.size(); ++i) {
		vol += data.tonewheels[i].curr_vol + data.tonewheels[i].dynamic_vol;
		sample += data.tonewheels[i].process(info, tonewheel_frequencies[i] * env.pitch_bend) * swell;
	}
	//Compress
	if (status.pressed_notes && data.preset.multi_note_gain != 1) {
		double v = calc_vol(status.pressed_notes, data.preset.multi_note_gain)/status.pressed_notes;
		vol *= v;
		sample *= v;
	}
	//Overdrive
	if (data.preset.overdrive) {
		sample = apply_distortion(sample, data.preset.overdrive, data.preset.distortion_type, data.preset.normalize_overdrive ? vol : 1);
	}

	//Rotary
	double lsample = sample;
	double rsample = sample;
	data.rotary_speaker.apply(lsample, rsample, data.preset.rotary, info);

	//Play
	for (size_t i = 0; i < channels.size() ; ++i) {
		channels[i] += i%2 == 0 ? lsample : rsample;
	}
}

void B3Organ::control_change(unsigned int control, unsigned int value) {
	//Drawbars
	for (size_t i = 0; i < data.preset.drawbar_ccs.size(); ++i) {
		if (data.preset.drawbar_ccs[i] == control) {
			data.preset.drawbars[i] = round((double) value/127 * ORGAN_DRAWBAR_MAX);
			submit_change(B3OrganProperty::pB3Drawbar1 + i, (int) data.preset.drawbars[i]);
		}
	}
	//Rotary
	if (control == data.preset.rotary_cc) {
		data.preset.rotary.on = value > 0;
		submit_change(B3OrganProperty::pB3Rotary, data.preset.rotary.on);
	}
	if (control == data.preset.rotary_speed_cc) {
		data.preset.rotary.fast = value > 0;
		submit_change(B3OrganProperty::pB3RotarySpeed, data.preset.rotary.fast);
	}
	//Overdrive
	if (control == data.preset.overdrive_cc) {
		data.preset.overdrive = value/127.0;
		submit_change(B3OrganProperty::pB3Overdrive, data.preset.overdrive);
	}
	//Percussion
	if (control == data.preset.percussion_cc) {
		data.preset.percussion = value > 0;
		submit_change(B3OrganProperty::pB3Percussion, data.preset.percussion);
	}
	//Percussion Third Harmonic
	if (control == data.preset.percussion_third_harmonic_cc) {
		data.preset.percussion_third_harmonic = value > 0;
		submit_change(B3OrganProperty::pB3PercussionThirdHarmonic, data.preset.percussion_third_harmonic);
	}
	//Percussion Fast Decay
	if (control == data.preset.percussion_fast_decay_cc) {
		data.preset.percussion_fast_decay = value > 0;
		submit_change(B3OrganProperty::pB3PercussionFastDecay, data.preset.percussion_fast_decay);
	}
	//Percussion Soft
	if (control == data.preset.percussion_soft_cc) {
		data.preset.percussion_soft = value > 0;
		submit_change(B3OrganProperty::pB3PercussionSoft, data.preset.percussion_soft);
	}
	//Swell
	if (control == data.preset.swell_cc) {
		data.swell = value/127.0;
	}
}

PropertyValue B3Organ::get(size_t prop) {
	PropertyValue val;
	switch ((B3OrganProperty) prop) {
	case B3OrganProperty::pB3Drawbar1:
		val.ival = data.preset.drawbars[0];
		break;
	case B3OrganProperty::pB3Drawbar2:
		val.ival = data.preset.drawbars[1];
		break;
	case B3OrganProperty::pB3Drawbar3:
		val.ival = data.preset.drawbars[2];
		break;
	case B3OrganProperty::pB3Drawbar4:
		val.ival = data.preset.drawbars[3];
		break;
	case B3OrganProperty::pB3Drawbar5:
		val.ival = data.preset.drawbars[4];
		break;
	case B3OrganProperty::pB3Drawbar6:
		val.ival = data.preset.drawbars[5];
		break;
	case B3OrganProperty::pB3Drawbar7:
		val.ival = data.preset.drawbars[6];
		break;
	case B3OrganProperty::pB3Drawbar8:
		val.ival = data.preset.drawbars[7];
		break;
	case B3OrganProperty::pB3Drawbar9:
		val.ival = data.preset.drawbars[8];
		break;
	case B3OrganProperty::pB3DrawbarCC1:
		val.ival = data.preset.drawbar_ccs[0];
		break;
	case B3OrganProperty::pB3DrawbarCC2:
		val.ival = data.preset.drawbar_ccs[1];
		break;
	case B3OrganProperty::pB3DrawbarCC3:
		val.ival = data.preset.drawbar_ccs[2];
		break;
	case B3OrganProperty::pB3DrawbarCC4:
		val.ival = data.preset.drawbar_ccs[3];
		break;
	case B3OrganProperty::pB3DrawbarCC5:
		val.ival = data.preset.drawbar_ccs[4];
		break;
	case B3OrganProperty::pB3DrawbarCC6:
		val.ival = data.preset.drawbar_ccs[5];
		break;
	case B3OrganProperty::pB3DrawbarCC7:
		val.ival = data.preset.drawbar_ccs[6];
		break;
	case B3OrganProperty::pB3DrawbarCC8:
		val.ival = data.preset.drawbar_ccs[7];
		break;
	case B3OrganProperty::pB3DrawbarCC9:
		val.ival = data.preset.drawbar_ccs[8];
		break;
	case B3OrganProperty::pB3HarmonicFoldbackVolume:
		val.dval = data.preset.harmonic_foldback_volume;
		break;
	case B3OrganProperty::pB3DistortionType:
		val.ival = data.preset.distortion_type;
		break;
	case B3OrganProperty::pB3NormalizeOverdrive:
		val.bval = data.preset.normalize_overdrive;
		break;
	case B3OrganProperty::pB3Overdrive:
		val.dval = data.preset.overdrive;
		break;
	case B3OrganProperty::pB3OverdriveCC:
		val.dval = data.preset.overdrive_cc;
		break;
	case B3OrganProperty::pB3MultiNoteGain:
		val.dval = data.preset.multi_note_gain;
		break;
	case B3OrganProperty::pB3Rotary:
		val.bval = data.preset.rotary.on;
		break;
	case B3OrganProperty::pB3RotarySpeed:
		val.bval = data.preset.rotary.fast;
		break;
	case B3OrganProperty::pB3RotaryCC:
		val.ival = data.preset.rotary_cc;
		break;
	case B3OrganProperty::pB3RotarySpeedCC:
		val.ival = data.preset.rotary_speed_cc;
		break;
	case B3OrganProperty::pB3RotaryStereoMix:
		val.dval = data.preset.rotary.stereo_mix;
		break;
	case B3OrganProperty::pB3RotaryGain:
		val.dval = data.preset.rotary.gain;
		break;
	case B3OrganProperty::pB3RotaryType:
		val.bval = data.preset.rotary.type;
		break;
	case B3OrganProperty::pB3RotaryDelay:
		val.ival = data.preset.rotary.max_delay;
		break;
	case B3OrganProperty::pB3Percussion:
		val.bval = data.preset.percussion;
		break;
	case B3OrganProperty::pB3PercussionThirdHarmonic:
		val.bval = data.preset.percussion_third_harmonic;
		break;
	case B3OrganProperty::pB3PercussionSoft:
		val.bval = data.preset.percussion_soft;
		break;
	case B3OrganProperty::pB3PercussionFastDecay:
		val.bval = data.preset.percussion_fast_decay;
		break;
	case B3OrganProperty::pB3PercussionSoftVolume:
		val.dval = data.preset.percussion_soft_volume;
		break;
	case B3OrganProperty::pB3PercussionHardVolume:
		val.dval = data.preset.percussion_hard_volume;
		break;
	case B3OrganProperty::pB3PercussionFastDecayTime:
		val.dval = data.preset.percussion_fast_decay_time;
		break;
	case B3OrganProperty::pB3PercussionSlowDecayTime:
		val.dval = data.preset.percussion_slow_decay_time;
		break;
	case B3OrganProperty::pB3PercussionCC:
		val.ival = data.preset.percussion_cc;
		break;
	case B3OrganProperty::pB3PercussionThirdHarmonicCC:
		val.ival = data.preset.percussion_third_harmonic_cc;
		break;
	case B3OrganProperty::pB3PercussionSoftCC:
		val.ival = data.preset.percussion_soft_cc;
		break;
	case B3OrganProperty::pB3PercussionFastDecayCC:
		val.ival = data.preset.percussion_fast_decay_cc;
		break;
	case B3OrganProperty::pB3SwellCC:
		val.ival = data.preset.swell_cc;
		break;
	}
	return val;
}

void B3Organ::set(size_t prop, PropertyValue val) {
	switch ((B3OrganProperty) prop) {
	case B3OrganProperty::pB3Drawbar1:
		data.preset.drawbars[0] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar2:
		data.preset.drawbars[1] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar3:
		data.preset.drawbars[2] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar4:
		data.preset.drawbars[3] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar5:
		data.preset.drawbars[4] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar6:
		data.preset.drawbars[5] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar7:
		data.preset.drawbars[6] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar8:
		data.preset.drawbars[7] = val.ival;
		break;
	case B3OrganProperty::pB3Drawbar9:
		data.preset.drawbars[8] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC1:
		data.preset.drawbar_ccs[0] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC2:
		data.preset.drawbar_ccs[1] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC3:
		data.preset.drawbar_ccs[2] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC4:
		data.preset.drawbar_ccs[3] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC5:
		data.preset.drawbar_ccs[4] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC6:
		data.preset.drawbar_ccs[5] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC7:
		data.preset.drawbar_ccs[6] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC8:
		data.preset.drawbar_ccs[7] = val.ival;
		break;
	case B3OrganProperty::pB3DrawbarCC9:
		data.preset.drawbar_ccs[8] = val.ival;
		break;
	case B3OrganProperty::pB3HarmonicFoldbackVolume:
		data.preset.harmonic_foldback_volume = val.dval;
		break;
	case B3OrganProperty::pB3DistortionType:
		data.preset.distortion_type = static_cast<DistortionType>(val.ival);
		break;
	case B3OrganProperty::pB3NormalizeOverdrive:
		data.preset.normalize_overdrive = val.bval;
		break;
	case B3OrganProperty::pB3Overdrive:
		data.preset.overdrive = val.dval;
		break;
	case B3OrganProperty::pB3OverdriveCC:
		data.preset.overdrive_cc = val.dval;
		break;
	case B3OrganProperty::pB3MultiNoteGain:
		data.preset.multi_note_gain = val.dval;
		break;
	case B3OrganProperty::pB3Rotary:
		data.preset.rotary.on = val.bval;
		break;
	case B3OrganProperty::pB3RotarySpeed:
		data.preset.rotary.fast = val.bval;
		break;
	case B3OrganProperty::pB3RotaryCC:
		data.preset.rotary_cc = val.ival;
		break;
	case B3OrganProperty::pB3RotarySpeedCC:
		data.preset.rotary_speed_cc = val.ival;
		break;
	case B3OrganProperty::pB3RotaryStereoMix:
		data.preset.rotary.stereo_mix = val.dval;
		break;
	case B3OrganProperty::pB3RotaryGain:
		data.preset.rotary.gain = val.dval;
		break;
	case B3OrganProperty::pB3RotaryType:
		data.preset.rotary.type = val.bval;
		break;
	case B3OrganProperty::pB3RotaryDelay:
		data.preset.rotary.max_delay = val.ival;
		break;
	case B3OrganProperty::pB3Percussion:
		data.preset.percussion = val.bval;
		break;
	case B3OrganProperty::pB3PercussionThirdHarmonic:
		data.preset.percussion_third_harmonic = val.bval;
		break;
	case B3OrganProperty::pB3PercussionSoft:
		data.preset.percussion_soft = val.bval;
		break;
	case B3OrganProperty::pB3PercussionFastDecay:
		data.preset.percussion_fast_decay = val.bval;
		break;
	case B3OrganProperty::pB3PercussionSoftVolume:
		data.preset.percussion_soft_volume = val.dval;
		break;
	case B3OrganProperty::pB3PercussionHardVolume:
		data.preset.percussion_hard_volume = val.dval;
		break;
	case B3OrganProperty::pB3PercussionFastDecayTime:
		data.preset.percussion_fast_decay_time = val.dval;
		break;
	case B3OrganProperty::pB3PercussionSlowDecayTime:
		data.preset.percussion_slow_decay_time = val.dval;
		break;
	case B3OrganProperty::pB3PercussionCC:
		data.preset.percussion_cc = val.ival;
		break;
	case B3OrganProperty::pB3PercussionThirdHarmonicCC:
		data.preset.percussion_third_harmonic_cc = val.ival;
		break;
	case B3OrganProperty::pB3PercussionSoftCC:
		data.preset.percussion_soft_cc = val.ival;
		break;
	case B3OrganProperty::pB3PercussionFastDecayCC:
		data.preset.percussion_fast_decay_cc = val.ival;
		break;
	case B3OrganProperty::pB3SwellCC:
		data.preset.swell_cc = val.ival;
		break;
	}
}

template<>
std::string get_engine_name<B3Organ>() {
	return "B3 Organ";
}

void __fix_link_organ_name__ () {
	get_engine_name<B3Organ>();
}
