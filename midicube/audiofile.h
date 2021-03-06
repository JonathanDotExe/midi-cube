/*
 * wav.h
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_AUDIOFILE_H_
#define MIDICUBE_AUDIOFILE_H_

#include <cstdint>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>


struct WAVHeader {
	char chunkID[4];
	std::uint32_t chunkSize;
	char riffType[4];
} __attribute__((packed));

struct WAVFormat {
	char chunkID[4];
	std::uint32_t chunkSize;
	std::uint16_t wFormatTag;
	std::uint16_t wChannels;
	std::uint32_t dwSamplesPerSec;
	std::uint32_t dwAvgBytesPerSec;
	std::uint16_t wBlockAlign;
	std::uint16_t wBitsPerSample;
} __attribute__((packed));

struct AudioSample {
	unsigned int sample_rate;
	unsigned int channels;
	std::vector<double> samples;

	double duration () {
		return sample_rate > 0 ? samples.size()/channels / (double) sample_rate : 0;
	}

	double sample(unsigned int channel, double time,unsigned int sample_rate ) {
		if (channels > 0) {
			channel %= channels;
			std::size_t index = (std::size_t) round(time * sample_rate) * channels + channel;
			return index < samples.size() ? samples[index] : 0;
		}
		else {
			return 0;
		}
	}

	double isample(unsigned int channel, double time, unsigned int sample_rate) {
		if (channels > 0) {
			channel %= channels;
			double index = time * sample_rate;
			std::size_t index1 = (std::size_t) floor(index)  * channels + channel;
			std::size_t index2 = (std::size_t) ceil(index) * channels + channel;
			double sample1 = index1 < samples.size() ? samples[index1] : 0;
			double sample2 = index2 < samples.size() ? samples[index2] : 0;
			double prog = 1 - index + floor(index);
			return sample1 * prog + sample2 * (1 - prog);
		}
		else {
			return 0;
		}
	}

	void clear () {
		sample_rate = 0;
		channels = 0;
		samples.clear();
	}
};

bool read_audio_file(AudioSample& audio, std::string fname);

bool write_audio_file(AudioSample& audio, std::string fname);


#endif /* MIDICUBE_AUDIOFILE_H_ */
