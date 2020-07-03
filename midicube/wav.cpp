/*
 * wav.cpp
 *
 *  Created on: 3 Jul 2020
 *      Author: jojo
 */

#include "wav.h"

#include <iostream>
#include <fstream>


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
		double max_value = 1 << format.wBitsPerSample;
		while (!in.eof()) {
			in.read(buffer, format.wBlockAlign);
			//To int
			std::uint64_t sample = 0;
			for (size_t i = 0; i < format.wBlockAlign; ++i) {
				sample += (std::uint32_t) buffer[format.wBlockAlign - 1 - i] << (i * 8);
			}
			audio.samples.push_back(sample/max_value * 2 - 1);
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

