/*
 * audiofle.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "audiofile.h"
#include <sndfile.h>
#include <sndfile.hh>
#include <iostream>

#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

bool read_audio_file(AudioSample& audio, std::string fname) {
	//Open
	SNDFILE* file = nullptr;
	SF_INFO info;
	SF_INSTRUMENT loop;
	bool success = true;

	file = sf_open(fname.c_str(), SFM_READ, &info);
	sf_command(file, SFC_GET_INSTRUMENT, &loop, sizeof(loop));

	//Info
	audio.channels = info.channels;
	audio.sample_rate = info.samplerate;
	if (loop.loop_count > 0) {
		audio.loop_start = loop.loops[0].start;
		audio.loop_end = loop.loops[0].end;
	}

	//Read
	if (file != nullptr) {
		double buffer[1024];
		sf_count_t size = _countof(buffer);
		sf_count_t count;
		do {
			count = sf_read_double(file, buffer, size);
			for (sf_count_t i = 0; i < count; i++) {
				audio.samples.push_back(buffer[i]);
			}
		} while (count >= size);
	}
	else {
		std::cerr << "Couldn't open sound file " << fname << ": " << sf_strerror(NULL) << std::endl;
		success = false;
	}

	if (file != nullptr) {
		sf_close(file);
		file = nullptr;
	}
	return success;
}

bool write_audio_file(AudioSample& audio, std::string fname) {
	SndfileHandle file = SndfileHandle(fname.c_str(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, audio.channels, audio.sample_rate);
	file.write(&audio.samples[0], audio.samples.size());

	return true; //TODO
}
