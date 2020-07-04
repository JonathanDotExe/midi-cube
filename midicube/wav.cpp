/*
 * wav.cpp
 *
 *  Created on: 3 Jul 2020
 *      Author: jojo
 */

#include "wav.h"

#include <iostream>
#include <fstream>

static void reverse (char arr[], std::size_t size) {
	size_t half = size/2;
	for (size_t i = 0; i < half; ++i) {
		char ch = arr[i];
		arr[i] = arr[size - 1 - i];
		arr[size - 1 - i] = ch;
	}
}

static std::int64_t twos_complement(std::uint64_t num, std::uint16_t bits) {
	std::uint64_t sign_mask = 1 << (bits - 1);
	if (!(num & sign_mask)) {
		//Positive
		return num;
	}
	//Negative
	std::uint64_t mask = 0;
	for (std::uint16_t i = 0; i < bits; ++i) {
		mask += 1 << i;
	}
	num = num & mask;
	num = !num;
	num = num & mask;
	num += 1;
	num = num & mask;
	return -num;
}

bool read_wav(WAVAudio& audio, std::string fname) {
	std::ifstream in;
	bool success = true;
	char* buffer = nullptr;
	try {
		in.open(fname.c_str(), std::ios::binary | std::ios::in);
		WAVHeader header;
		in.read((char*) &header, sizeof(header));
		WAVFormat format;
		in.read((char*) &format, sizeof(format));
		audio.sample_rate = format.dwSamplesPerSec;
		audio.channels = format.wChannels;

		//Samples
		buffer = new char[format.wBlockAlign];
		double max_value = (std::uint32_t) (1 << format.wBitsPerSample);
		std::cout << format.wBitsPerSample << " " << max_value << std::endl;
		while (!in.eof()) {
			in.read(buffer, format.wBlockAlign);
			//Reverse buffer
			reverse(buffer, sizeof(buffer));
			//To int
			std::uint64_t sample = 0;
			for (size_t i = 0; i < format.wBitsPerSample; ++i) {
				size_t bit = format.wBitsPerSample - 1 - i;
				size_t byte = bit/8;
				sample += (std::uint64_t) (buffer[byte] & (1 << (bit % 8))) << byte;
			}
			audio.samples.push_back(twos_complement(sample, format.wBitsPerSample)/max_value * 2);
		}
	}
	catch (const std::ifstream::failure& e) {
		std::cerr << e.what() << std::endl;
		success = false;
	}
	delete buffer;
	buffer = nullptr;

	return success;
}

