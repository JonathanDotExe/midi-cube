/*
 * vocoder.cpp
 *
 *  Created on: Dec 3, 2020
 *      Author: jojo
 */

#include "vocoder.h"

VocoderEffect::VocoderEffect() {
	//Set up bands
	for (size_t i = 0; i < bands.size(); ++i) {
		VocoderBand& band = bands.at(i);
		band.filter_data.cutoff = /*(VOCODER_HIGH_BAND - VOCODER_LOW_BAND)/VOCODER_BAND_COUNT * i + VOCODER_LOW_BAND*/ frequencies[i];
		band.filter_data.type = FilterType::BP_12;
		band.filter_data.resonance = 0.7;
	}
}

void VocoderEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	double modulator = info.input_sample;

	if (preset.on) {
		//Calc mix
		double total_mix = preset.carrier_amp + preset.vocoder_amp + preset.voice_amp;
		double carrier_mix = 0;
		double vocoder_mix = 0;
		double modulator_mix = 0;
		if (total_mix) {
			carrier_mix = preset.carrier_amp/total_mix;
			vocoder_mix = preset.vocoder_amp/total_mix;
			modulator_mix = preset.voice_amp/total_mix;
		}
		//Gate
		modulator *= preset.modulator_amplification;
		modulator_env.apply(modulator, info.time_step);
		if (modulator_env.volume() < preset.gate) {
			modulator = 0;
		}
		//Vocode
		double lvocoded = 0;
		double rvocoded = 0;
		//Filters
		double vol_sum = 0;

		for (size_t i = 0; i < bands.size(); ++i) {
			VocoderBand& band = bands[i];

			FilterData filter_data{FilterType::BP_12, preset.formant_mode ? frequencies[i] : ((preset.max_freq - preset.min_freq)/VOCODER_BAND_COUNT * i + preset.min_freq), preset.resonance};
			//Filter
			double lf = band.lfilter.apply(filter_data, lsample, info.time_step);
			double rf = band.rfilter.apply(filter_data, rsample, info.time_step);
			double m = band.mfilter.apply(filter_data, modulator, info.time_step);

			//Modulator amp
			band.env.apply(m, info.time_step);
			double vol = band.env.volume();

			band.port.set(vol, info.time, preset.slope);
			vol = band.port.get(info.time);
			vol_sum += vol;

			//Vocode carrier
			lvocoded += lf * vol;
			rvocoded += rf * vol;
		}

		if (vol_sum) {
			lvocoded /= vol_sum + (1 - vol_sum) * (1 - preset.compression);
			rvocoded /= vol_sum + (1 - vol_sum) * (1 - preset.compression);
		}
		//Highpass
		if (preset.mod_highpass) {
			FilterData data = {FilterType::HP_12, preset.mod_highpass};
			modulator = mfilter.apply(data, modulator, info.time_step);
		}
		//Mix
		lsample *= carrier_mix;
		rsample *= carrier_mix;
		lsample += lvocoded * vocoder_mix;
		rsample += rvocoded * vocoder_mix;
		lsample += modulator * modulator_mix;
		rsample += modulator * modulator_mix;

		lsample *= preset.post_amplification;
		rsample *= preset.post_amplification;
	}
}

template <>
EffectProgram* create_effect_program<VocoderEffect>() {
	return nullptr;
}


template<>
std::string get_effect_name<VocoderEffect>() {
	return "Vocoder";
}
