/*
 * drums.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */


#include "drums.h"


//SampleDrums
SampleDrums::SampleDrums () {
	//Create Test drumkit for testing purposes
	//TODO create universal drumkit loader
	drumkit = new SampleDrumKit();
	drumkit->notes[44] = {};
	read_audio_file(drumkit->notes[44], "./data/drumkits/test/ride.wav");
	drumkit->notes[45] = {};
	read_audio_file(drumkit->notes[45], "./data/drumkits/test/snare.wav");
	drumkit->notes[46] = {};
	read_audio_file(drumkit->notes[46], "./data/drumkits/test/hi_hat.wav");
	drumkit->notes[47] = {};
	read_audio_file(drumkit->notes[47], "./data/drumkits/test/bass.wav");
	drumkit->notes[48] = {};
	read_audio_file(drumkit->notes[48], "./data/drumkits/test/percussion1.wav");
	drumkit->notes[49] = {};
	read_audio_file(drumkit->notes[49], "./data/drumkits/test/crash1.wav");
	drumkit->notes[50] = {};
	read_audio_file(drumkit->notes[50], "./data/drumkits/test/tom1.wav");
	drumkit->notes[51] = {};
	read_audio_file(drumkit->notes[51], "./data/drumkits/test/tom3.wav");
}

void SampleDrums::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index) {
	try {
		if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
			AudioSample& audio = drumkit->notes[note.note];
			for (size_t i = 0; i < channels.size(); ++i) {
				channels[i] += audio.sample(i, info.time - note.start_time, info.sample_rate);
			}
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		throw e;
	}
}

bool SampleDrums::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data) {
	if (drumkit->notes.find(note.note) != drumkit->notes.end()) {
		return info.time - note.start_time > (double) drumkit->notes[note.note].duration();
	}
	return true;
}

std::string SampleDrums::get_name() {
	return "Sample Drums";
}

SampleDrums::~SampleDrums() {
	delete drumkit;
	drumkit = nullptr;
}
