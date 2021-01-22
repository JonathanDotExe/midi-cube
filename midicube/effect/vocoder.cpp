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
		band.filter_data.cutoff = cutoff_to_factor((VOCODER_HIGH_BAND - VOCODER_LOW_BAND)/VOCODER_BAND_COUNT * i + VOCODER_LOW_BAND, 1/44100.0); //TODO dependent on the sampel rate
		band.filter_data.type = FilterType::LP_24;
	}
}

void VocoderEffect::apply(double& lsample, double& rsample, double modulator, VocoderPreset& preset, SampleInfo& info) {
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
		modulator *= preset.mod_pre_amp;
		modulator_env.apply(modulator, info.time_step);
		if (modulator_env.volume() < preset.gate) {
			modulator = 0;
		}
		//Vocode
		double lvocoded = 0;
		double rvocoded = 0;
		if (modulator && (lsample || rsample)) {
			//Filters
			std::array<double, VOCODER_BAND_COUNT> lfiltered;
			std::array<double, VOCODER_BAND_COUNT> rfiltered;
			std::array<double, VOCODER_BAND_COUNT> mfiltered;
			lfiltered[0] = bands[0].lfilter.apply(bands[0].filter_data, lsample, info.time_step);
			rfiltered[0] = bands[0].rfilter.apply(bands[0].filter_data, rsample, info.time_step);
			mfiltered[0] = bands[0].rfilter.apply(bands[0].filter_data, modulator, info.time_step);
			for (size_t i = 1; i < bands.size(); ++i) {
				VocoderBand& band = bands[i];
				//Filter
				double lf = band.lfilter.apply(band.filter_data, lsample, info.time_step);
				double rf = band.rfilter.apply(band.filter_data, rsample, info.time_step);
				double m = band.rfilter.apply(band.filter_data, modulator, info.time_step);
				lfiltered[i] = lf;
				rfiltered[i] = rf;
				mfiltered[i] = m;

				lf -= lfiltered[i - 1];
				rf -= rfiltered[i - 1];
				m -= mfiltered[i - 1];

				//Modulator amp
				band.env.apply(m, info.time_step);
				double vol = band.env.volume();

				//Vocode carrier
				lvocoded += lfiltered[i] * vol;
				rvocoded += rfiltered[i] * vol;
			}
		}
		//Highpass
		if (preset.mod_highpass) {
			FilterData data = {FilterType::HP_12, cutoff_to_factor(preset.mod_highpass, info.time_step)};
			modulator = mfilter.apply(data, modulator, info.time_step);
		}
		//Mix
		lsample *= carrier_mix;
		rsample *= carrier_mix;
		lsample += lvocoded * vocoder_mix;
		rsample += rvocoded * vocoder_mix;
		lsample += modulator * modulator_mix;
		rsample += modulator * modulator_mix;
	}
}
