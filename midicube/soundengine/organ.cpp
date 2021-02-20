/*
 * organ.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "organ.h"
#include <cmath>

//B3OrganTonewheel
double B3OrganTonewheel::process(SampleInfo& info, double freq) {
	//Rotation
	double volume = this->volume;
	rotation += freq * info.time_step;
	this->volume = 0;
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
		double curr_vol = db_to_amp((-3 * ((int) i/12 + 1)));
		tonewheel_data[i + start_tw].volume = curr_vol;
	}
}

void B3Organ::trigger_tonewheel(int tonewheel, double volume, SampleInfo& info, TriggeredNote& note) {
	bool vol_mul = 1;
	while (tonewheel < 0) {
		tonewheel += 12;
		vol_mul = data.preset.harmonic_foldback_volume;
	}
	while (tonewheel >= (int) data.tonewheels.size()) {
		tonewheel -= 12;
		vol_mul = data.preset.harmonic_foldback_volume;
	}
	if (tonewheel >= 0 && info.time >= note.start_time + tonewheel_data[tonewheel].press_delay && (note.pressed || info.time <= note.release_time + tonewheel_data[tonewheel].release_delay)) {
		data.tonewheels[tonewheel].volume += volume * vol_mul;
	}
}

void B3Organ::process_note_sample(double& lsample, double& rsample, SampleInfo &info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Organ sound
	double drawbar_amount = data.preset.drawbars.size() + (data.preset.percussion_soft ? data.preset.percussion_soft_volume : data.preset.percussion_hard_volume);
	for (size_t i = 0; i < ORGAN_DRAWBAR_COUNT; ++i) {
		int tonewheel = note.note + drawbar_notes[i] - ORGAN_LOWEST_TONEWHEEL_NOTE;
		trigger_tonewheel(tonewheel, data.preset.drawbars[i] / (double) ORGAN_DRAWBAR_MAX / drawbar_amount, info, note);
	}
	//Percussion
	double decay = data.preset.percussion_fast_decay ? data.preset.percussion_fast_decay_time : data.preset.percussion_slow_decay_time;
	if (data.preset.percussion && info.time - data.percussion_start <= decay) {
		double vol = (1 - (info.time - data.percussion_start)/decay) * (data.preset.percussion_soft ? data.preset.percussion_soft_volume : data.preset.percussion_hard_volume);
		vol /= drawbar_amount;
		int tonewheel = note.note + (data.preset.percussion_third_harmonic ? 19 : 12) - ORGAN_LOWEST_TONEWHEEL_NOTE;

		trigger_tonewheel(tonewheel, vol, info, note);
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
	return !note.pressed && info.time > ORGAN_MAX_UP_DELAY;
};

void B3Organ::process_sample(double& lsample, double& rsample, SampleInfo &info, KeyboardEnvironment& env, EngineStatus& status) {
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
		volume += data.tonewheels[i].volume * tonewheel_data[i].volume;
		sample += data.tonewheels[i].process(info, tonewheel_data[i].freq * env.pitch_bend) * tonewheel_data[i].volume;
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
	data.amplifier.apply(lsample, rsample, data.preset.amplifier, info);

	//Rotary
	data.rotary_speaker.apply(lsample, rsample, data.preset.rotary, info);
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
	//Amp
	if (control == data.preset.amp_cc) {
		data.preset.amplifier.on = value > 0;
		submit_change(B3OrganProperty::pB3AmpOn, data.preset.amplifier.on);
	}
	//Boost
	if (control == data.preset.amp_boost_cc) {
		data.preset.amplifier.boost = value/127.0;
		submit_change(B3OrganProperty::pB3AmpBoost, data.preset.amplifier.boost);
	}
	//Drive
	if (control == data.preset.amp_drive_cc) {
		data.preset.amplifier.drive = value/127.0;
		submit_change(B3OrganProperty::pB3AmpDrive, data.preset.amplifier.drive);
	}
	//Tone
	if (control == data.preset.amp_tone_cc) {
		data.preset.amplifier.tone = value/127.0;
		submit_change(B3OrganProperty::pB3AmpTone, data.preset.amplifier.tone);
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
	//Vibrato Mix
	if (control == data.preset.vibrato_mix_cc) {
		data.preset.vibrato_mix = value/127.0;
		submit_change(B3OrganProperty::pB3VibratoMix, data.preset.vibrato_mix);
	}
	//Swell
	if (control == data.preset.swell_cc) {
		data.swell = value/127.0;
	}
}

PropertyValue B3Organ::get(size_t prop, size_t sub_prop) {
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
	case B3OrganProperty::pB3AmpOn:
		val.bval = data.preset.amplifier.on;
		break;
	case B3OrganProperty::pB3AmpDistortionType:
		val.ival = data.preset.amplifier.type;
		break;
	case B3OrganProperty::pB3AmpDrive:
		val.dval = data.preset.amplifier.drive;
		break;
	case B3OrganProperty::pB3AmpTone:
		val.dval = data.preset.amplifier.tone;
		break;
	case B3OrganProperty::pB3AmpOnCC:
		val.ival = data.preset.amp_cc;
		break;
	case B3OrganProperty::pB3AmpBoostCC:
		val.ival = data.preset.amp_boost_cc;
		break;
	case B3OrganProperty::pB3AmpDriveCC:
		val.ival = data.preset.amp_drive_cc;
		break;
	case B3OrganProperty::pB3AmpToneCC:
		val.ival = data.preset.amp_tone_cc;
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
	case B3OrganProperty::pB3AmpBoost:
		val.dval = data.preset.amplifier.boost;
		break;
	case B3OrganProperty::pB3RotaryType:
		val.bval = data.preset.rotary.type;
		break;
	case B3OrganProperty::pB3RotaryDelay:
		val.dval = data.preset.rotary.max_delay;
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
	case B3OrganProperty::pB3VibratoMix:
		val.dval = data.preset.vibrato_mix;
		break;
	case B3OrganProperty::pB3VibratoMixCC:
		val.ival = data.preset.vibrato_mix_cc;
		break;
	case B3OrganProperty::pB3SwellCC:
		val.ival = data.preset.swell_cc;
		break;
	}
	return val;
}

void B3Organ::set(size_t prop, PropertyValue val, size_t sub_prop) {
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
	case B3OrganProperty::pB3AmpOn:
		data.preset.amplifier.on = val.bval;
		break;
	case B3OrganProperty::pB3AmpDistortionType:
		data.preset.amplifier.type = static_cast<DistortionType>(val.ival);
		break;
	case B3OrganProperty::pB3AmpDrive:
		data.preset.amplifier.drive = val.dval;
		break;
	case B3OrganProperty::pB3AmpTone:
		data.preset.amplifier.tone = val.dval;
		break;
	case B3OrganProperty::pB3AmpOnCC:
		data.preset.amp_cc = val.ival;
		break;
	case B3OrganProperty::pB3AmpBoostCC:
		data.preset.amp_boost_cc = val.ival;
		break;
	case B3OrganProperty::pB3AmpDriveCC:
		data.preset.amp_drive_cc = val.ival;
		break;
	case B3OrganProperty::pB3AmpToneCC:
		data.preset.amp_tone_cc = val.ival;
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
	case B3OrganProperty::pB3AmpBoost:
		data.preset.amplifier.boost = val.dval;
		break;
	case B3OrganProperty::pB3RotaryType:
		data.preset.rotary.type = val.bval;
		break;
	case B3OrganProperty::pB3RotaryDelay:
		data.preset.rotary.max_delay = val.dval;
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
	case B3OrganProperty::pB3VibratoMix:
		data.preset.vibrato_mix = val.dval;
		break;
	case B3OrganProperty::pB3VibratoMixCC:
		data.preset.vibrato_mix_cc = val.ival;
		break;
	case B3OrganProperty::pB3SwellCC:
		data.preset.swell_cc = val.ival;
		break;
	}
}

void B3Organ::update_properties() {
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
	submit_change(B3OrganProperty::pB3AmpOn, data.preset.amplifier.on);
	submit_change(B3OrganProperty::pB3AmpDistortionType,
			data.preset.amplifier.type);
	submit_change(B3OrganProperty::pB3AmpDrive, data.preset.amplifier.drive);
	submit_change(B3OrganProperty::pB3AmpTone, data.preset.amplifier.tone);
	submit_change(B3OrganProperty::pB3AmpOnCC,
			(int) data.preset.amp_cc);
	submit_change(B3OrganProperty::pB3AmpBoostCC,
			(int) data.preset.amp_boost_cc);
	submit_change(B3OrganProperty::pB3AmpDriveCC,
			(int) data.preset.amp_drive_cc);
	submit_change(B3OrganProperty::pB3AmpToneCC,
			(int) data.preset.amp_tone_cc);
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
	submit_change(B3OrganProperty::pB3AmpBoost, data.preset.amplifier.boost);
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
	submit_change(B3OrganProperty::pB3VibratoMix,
			data.preset.vibrato_mix);
	submit_change(B3OrganProperty::pB3VibratoMixCC,
			(int) data.preset.vibrato_mix_cc);
	submit_change(B3OrganProperty::pB3SwellCC,
			(int) data.preset.swell_cc);
}

template<>
std::string get_engine_name<B3Organ>() {
	return "B3 Organ";
}

void __fix_link_organ_name__ () {
	get_engine_name<B3Organ>();
}
