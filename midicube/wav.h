/*
 * wav.h
 *
 *  Created on: 1 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_WAV_H_
#define MIDICUBE_WAV_H_

#include <cstdint>
#include <vector>
#include <string>


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

struct WAVAudio {
	unsigned int sample_rate;
	unsigned int channels;
	std::vector<double> samples;
};

bool read_wav(WAVAudio& audio, std::string fname);


#endif /* MIDICUBE_WAV_H_ */
