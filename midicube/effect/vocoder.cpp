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
		band.filter_data.type == FilterType::BP_12;
	}
}

double VocoderEffect::apply(double lsample, double rsample, VocoderPreset& preset) {

}
